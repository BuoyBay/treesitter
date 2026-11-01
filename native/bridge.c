/* Stable scalar/pointer ABI for Buoy. Owners must outlive borrowed objects. */
#include <tree_sitter/api.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define BTS_EXPORT __declspec(dllexport)
typedef HMODULE BTSLibrary;
#else
#include <dlfcn.h>
#define BTS_EXPORT __attribute__((visibility("default")))
typedef void *BTSLibrary;
#endif

typedef struct {
    BTSLibrary library;
    const TSLanguage *language;
    int owns_library;
} BTSLanguage;

typedef struct {
    TSQuery *query;
    char **names;
    uint32_t name_count;
} BTSQuery;

typedef struct {
    TSQueryCursor *cursor;
    const BTSQuery *query;
    TSQueryMatch match;
    uint32_t capture_index;
    int has_capture;
} BTSCursor;

static int u32(int64_t n) { return n >= 0 && (uint64_t)n <= UINT32_MAX; }

static TSNode *node_new(TSNode node) {
    if (ts_node_is_null(node)) return NULL;
    TSNode *result = malloc(sizeof(*result));
    if (result) *result = node;
    return result;
}

BTS_EXPORT void *bts_language_open(const char *path, const char *symbol) {
    if (!path || !symbol || !*symbol) return NULL;
    BTSLanguage *result = calloc(1, sizeof(*result));
    if (!result) return NULL;
#ifdef _WIN32
    if (*path) {
        int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
        if (!length) { free(result); return NULL; }
        wchar_t *wide = malloc((size_t)length * sizeof(*wide));
        if (!wide) { free(result); return NULL; }
        if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, length)) {
            free(wide); free(result); return NULL;
        }
        result->library = LoadLibraryW(wide);
        free(wide);
        result->owns_library = 1;
    } else {
        result->library = GetModuleHandleW(NULL);
    }
    FARPROC address = result->library ? GetProcAddress(result->library, symbol) : NULL;
    const TSLanguage *(*factory)(void) = NULL;
    if (address) memcpy(&factory, &address, sizeof(factory));
#else
    result->library = dlopen(*path ? path : NULL, RTLD_NOW | RTLD_LOCAL);
    result->owns_library = 1;
    void *address = result->library ? dlsym(result->library, symbol) : NULL;
    const TSLanguage *(*factory)(void) = NULL;
    /* POSIX permits dlsym function pointers; memcpy avoids aliasing warnings. */
    if (address) memcpy(&factory, &address, sizeof(factory));
#endif
    if (factory) result->language = factory();
    if (!result->language) {
#ifdef _WIN32
        if (result->library && result->owns_library) FreeLibrary(result->library);
#else
        if (result->library) dlclose(result->library);
#endif
        free(result);
        return NULL;
    }
    return result;
}

BTS_EXPORT void bts_language_close(BTSLanguage *language) {
    if (!language) return;
#ifdef _WIN32
    if (language->owns_library) FreeLibrary(language->library);
#else
    if (language->owns_library) dlclose(language->library);
#endif
    free(language);
}

BTS_EXPORT void *bts_parser_new(const BTSLanguage *language) {
    if (!language) return NULL;
    TSParser *parser = ts_parser_new();
    if (parser && !ts_parser_set_language(parser, language->language)) {
        ts_parser_delete(parser);
        return NULL;
    }
    return parser;
}
BTS_EXPORT void bts_parser_delete(TSParser *parser) { if (parser) ts_parser_delete(parser); }
BTS_EXPORT void *bts_parse(TSParser *parser, const TSTree *old_tree, const char *source, int64_t length) {
    if (!parser || !source || !u32(length)) return NULL;
    return ts_parser_parse_string(parser, old_tree, source, (uint32_t)length);
}
BTS_EXPORT void bts_tree_delete(TSTree *tree) { if (tree) ts_tree_delete(tree); }
BTS_EXPORT void *bts_tree_copy(const TSTree *tree) { return tree ? ts_tree_copy(tree) : NULL; }
BTS_EXPORT void *bts_tree_root(const TSTree *tree) { return tree ? node_new(ts_tree_root_node(tree)) : NULL; }
BTS_EXPORT void bts_node_delete(TSNode *node) { free(node); }
BTS_EXPORT const char *bts_node_type(const TSNode *node) { return node ? ts_node_type(*node) : ""; }

#define NODE_SCALAR(name, expression) \
    BTS_EXPORT int64_t bts_node_##name(const TSNode *node) { return node ? (expression) : 0; }
NODE_SCALAR(start_byte, ts_node_start_byte(*node))
NODE_SCALAR(end_byte, ts_node_end_byte(*node))
NODE_SCALAR(start_row, ts_node_start_point(*node).row)
NODE_SCALAR(start_column, ts_node_start_point(*node).column)
NODE_SCALAR(end_row, ts_node_end_point(*node).row)
NODE_SCALAR(end_column, ts_node_end_point(*node).column)
NODE_SCALAR(child_count, ts_node_child_count(*node))
NODE_SCALAR(named_child_count, ts_node_named_child_count(*node))
NODE_SCALAR(is_named, ts_node_is_named(*node))
NODE_SCALAR(has_error, ts_node_has_error(*node))
NODE_SCALAR(is_missing, ts_node_is_missing(*node))

BTS_EXPORT void *bts_node_child(const TSNode *node, int64_t index, int64_t named) {
    if (!node || !u32(index)) return NULL;
    return node_new(named ? ts_node_named_child(*node, (uint32_t)index) : ts_node_child(*node, (uint32_t)index));
}
BTS_EXPORT void *bts_node_parent(const TSNode *node) { return node ? node_new(ts_node_parent(*node)) : NULL; }
BTS_EXPORT void *bts_node_child_by_field(const TSNode *node, const char *name) {
    if (!node || !name || strlen(name) > UINT32_MAX) return NULL;
    return node_new(ts_node_child_by_field_name(*node, name, (uint32_t)strlen(name)));
}
BTS_EXPORT char *bts_node_string(const TSNode *node) { return node ? ts_node_string(*node) : NULL; }
BTS_EXPORT void bts_free(void *memory) { free(memory); }

static int point_before(TSPoint a, TSPoint b) {
    return a.row < b.row || (a.row == b.row && a.column <= b.column);
}
BTS_EXPORT int64_t bts_tree_edit(TSTree *tree, int64_t start, int64_t old_end, int64_t new_end,
    int64_t start_row, int64_t start_column, int64_t old_row, int64_t old_column,
    int64_t new_row, int64_t new_column) {
    if (!tree || !u32(start) || !u32(old_end) || !u32(new_end) ||
        !u32(start_row) || !u32(start_column) || !u32(old_row) || !u32(old_column) ||
        !u32(new_row) || !u32(new_column) || old_end < start || new_end < start) return 0;
    TSInputEdit edit = {(uint32_t)start, (uint32_t)old_end, (uint32_t)new_end,
        {(uint32_t)start_row, (uint32_t)start_column},
        {(uint32_t)old_row, (uint32_t)old_column},
        {(uint32_t)new_row, (uint32_t)new_column}};
    if (!point_before(edit.start_point, edit.old_end_point) ||
        !point_before(edit.start_point, edit.new_end_point)) return 0;
    ts_tree_edit(tree, &edit);
    return 1;
}

BTS_EXPORT void bts_query_delete(BTSQuery *query) {
    if (!query) return;
    if (query->names) for (uint32_t i = 0; i < query->name_count; i++) free(query->names[i]);
    free(query->names);
    if (query->query) ts_query_delete(query->query);
    free(query);
}

/* Custom errors: 100 unsupported predicates, 101 allocation, 102 invalid input. */
BTS_EXPORT void *bts_query_new(const BTSLanguage *language, const char *source, int64_t length,
    int64_t *offset, int64_t *error) {
    if (offset) *offset = 0;
    if (error) *error = 0;
    if (!language || !source || !u32(length)) { if (error) *error = 102; return NULL; }
    uint32_t native_offset = 0;
    TSQueryError native_error = TSQueryErrorNone;
    TSQuery *native = ts_query_new(language->language, source, (uint32_t)length, &native_offset, &native_error);
    if (!native) {
        if (offset) *offset = native_offset;
        if (error) *error = native_error;
        return NULL;
    }
    for (uint32_t i = 0; i < ts_query_pattern_count(native); i++) {
        uint32_t count = 0;
        ts_query_predicates_for_pattern(native, i, &count);
        if (count) {
            if (offset) *offset = ts_query_start_byte_for_pattern(native, i);
            if (error) *error = 100;
            ts_query_delete(native);
            return NULL;
        }
    }
    BTSQuery *query = calloc(1, sizeof(*query));
    if (!query) { ts_query_delete(native); if (error) *error = 101; return NULL; }
    query->query = native;
    query->name_count = ts_query_capture_count(native);
    if (query->name_count) {
        query->names = calloc(query->name_count, sizeof(*query->names));
        if (!query->names) goto allocation_failure;
    }
    for (uint32_t i = 0; i < query->name_count; i++) {
        uint32_t size = 0;
        const char *name = ts_query_capture_name_for_id(native, i, &size);
        query->names[i] = malloc((size_t)size + 1);
        if (!query->names[i]) goto allocation_failure;
        memcpy(query->names[i], name, size);
        query->names[i][size] = 0;
    }
    return query;
allocation_failure:
    if (error) *error = 101;
    bts_query_delete(query);
    return NULL;
}

BTS_EXPORT void *bts_query_cursor_new(const BTSQuery *query, const TSNode *node) {
    if (!query || !node) return NULL;
    BTSCursor *cursor = calloc(1, sizeof(*cursor));
    if (!cursor) return NULL;
    cursor->cursor = ts_query_cursor_new();
    if (!cursor->cursor) { free(cursor); return NULL; }
    cursor->query = query;
    ts_query_cursor_exec(cursor->cursor, query->query, *node);
    return cursor;
}
BTS_EXPORT void bts_query_cursor_delete(BTSCursor *cursor) {
    if (!cursor) return;
    ts_query_cursor_delete(cursor->cursor);
    free(cursor);
}
BTS_EXPORT int64_t bts_query_next(BTSCursor *cursor) {
    if (!cursor) return 0;
    cursor->has_capture = ts_query_cursor_next_capture(cursor->cursor, &cursor->match, &cursor->capture_index);
    return cursor->has_capture;
}
BTS_EXPORT void *bts_query_capture_node(const BTSCursor *cursor) {
    return cursor && cursor->has_capture ? node_new(cursor->match.captures[cursor->capture_index].node) : NULL;
}
BTS_EXPORT const char *bts_query_capture_name(const BTSCursor *cursor) {
    if (!cursor || !cursor->has_capture) return "";
    uint32_t id = cursor->match.captures[cursor->capture_index].index;
    return id < cursor->query->name_count ? cursor->query->names[id] : "";
}
BTS_EXPORT int64_t bts_query_pattern(const BTSCursor *cursor) {
    return cursor && cursor->has_capture ? cursor->match.pattern_index : -1;
}
