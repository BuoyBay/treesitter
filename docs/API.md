# Tree-sitter for Buoy

A cross-platform `treesitter` package wrapping the native tree-sitter runtime.
It loads compiled grammars and exposes managed parsers, trees, nodes and structural
query captures. The runtime is embedded in the bridge; consumers do not need a
separate tree-sitter installation.

## Build and test

Requires Buoy 0.72+, Python 3.9+, a C11 compiler, and network access for the first
build. Upstream sources are pinned to tree-sitter 0.25.10 and tree-sitter-json
0.24.8 with SHA256 verification. Subsequent builds reuse verified cached archives.

```sh
python3 tools/build_treesitter.py
```

The build produces `packages/treesitter-0.1.0.buipkg`, a copy under `dist/`, and
a separate JSON grammar fixture under `dist/treesitter/<target>/`. Copy the
package into a consumer project's `packages/` directory, then `Import treesitter`.
The JSON grammar is a test/example dependency, not bundled into the reusable runtime.

Run the builder on each desired OS/architecture, collect its `runtime/<target>`
and Windows `link/<target>` directories into the package source, then seal with
`python3 tools/build_treesitter.py --seal-only`. The archive advertises only
runtime slices actually present. Generated native slices are ignored by Git;
the sealed package is the distributable artifact.

macOS builds use Clang and a deployment target of macOS 11+. Windows builds
require a Visual Studio developer environment with `cl` or `--cc clang-cl` and
the matching architecture/SDK. A MinGW cross compiler is also supported, for
example `--target x86_64-windows --cc x86_64-w64-mingw32-gcc`. Linux builds
use `cc` and `libdl`. `--target`
selects an architecture but does not install a cross compiler or SDK. Use
`--cc` and repeated `--cflag` arguments to configure a compatible cross compiler.
`BUOY` selects the compiler executable. All six Buoy target branches are checked
when sealing, independently of which native slices are present.

## API

See [the runnable example](../examples/treesitter_json.bui) for complete Buoy
source. Its argument is the absolute path to the JSON grammar shared library
built by the script. Grammars must export a factory such as `tree_sitter_json`
and have an ABI accepted by the pinned runtime. Load only trusted native libraries.
An empty library path searches the main process; normally use an absolute path.
This wrapper loads existing grammars; it does not generate a Buoy grammar.

| Object | Operations |
| --- | --- |
| `TSLanguage(path, symbol)` | Load a grammar `.dylib`, `.so` or `.dll`; retained automatically by dependents |
| `TSParser(language)` | `Parse(source)`, `ParseIncremental(source, oldTree)`, `Close()` |
| `TSTree` | `RootNode()`, `Copy()`, `Edit(...)`, `Close()` |
| `TSNode` | `Child(index, named)`, `Parent()`, `ChildByField(name)`, `SExpression()`, `Close()` |
| `TSQuery(language, source)` | Compile a structural query; `Captures(node)` returns a cursor |
| `TSCaptureCursor` | `MoveNext()`, `Name`, `PatternIndex`, `Node()`, `Close()` |

Nodes expose `TypeName`, `StartByte`, `EndByte`, `StartRow`, `StartColumn`,
`EndRow`, `EndColumn`, `ChildCount`, `NamedChildCount`, `IsNamed`, `HasError` and
`IsMissing`. Missing children, parents and fields return `Nil`. Children use
zero-based indices. All source offsets, point columns and edit coordinates are
**UTF-8 bytes**, not Buoy character indices; rows are zero-based and end offsets
are exclusive. Parse errors remain in the syntax tree (`HasError`).

`Edit(startByte, oldEndByte, newEndByte, startRow, startColumn, oldEndRow,
oldEndColumn, newEndRow, newEndColumn)` updates an old tree before passing it to
`ParseIncremental`. Supply coordinates matching the actual source change. The
wrapper validates uint32 bounds and ordering, but does not keep source text to
verify the edit against it. A fresh parse remains available with `Parse`.

## Ownership and limits

ARC releases native allocations automatically. Explicit `Close()` is idempotent
on parsers, trees, nodes and cursors. Nodes retain their tree, trees and queries
retain their language, and capture cursors retain their query and root node.
Closing a tree invalidates its nodes/cursors; editing a tree invalidates existing
nodes/cursors through a generation check. A copied tree remains independent.
Closing a parser leaves its returned trees usable. Languages and queries have
ARC-only lifetimes so explicit closure cannot invalidate dependent objects.

Confine each object graph to one thread, or synchronize externally. Queries
support structural patterns and captures; predicates/directives (including
`#eq?`, `#match?` and `#set!`) reject with error 100. The C runtime does not
evaluate these, so accepting them would silently produce incorrect results.
Other query errors include the native error code and byte offset. This release
does not expose streaming input, included ranges, cancellation, query match
limits, changed ranges or tree-walking cursors. Query capture iteration can be
expensive on untrusted patterns or large inputs.

The upstream runtime and JSON fixture are MIT-licensed. Their license texts
are copied from the verified source archives when building; the runtime license
is delivered with the package under `resources/treesitter-licenses/`.

Builder context: run shared `tools/` commands from the assembled
[BuoyBay workspace](https://github.com/BuoyBay/workspace-tools). The scripts live
in [devtools](https://github.com/BuoyBay/devtools), alongside the shared build inputs.
