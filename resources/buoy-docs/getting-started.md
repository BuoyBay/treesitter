---
title: "Tree-sitter getting started"
summary: "Parse JSON using a trusted grammar."
category: "Guides"
order: 10
keywords: ["tree-sitter", "parser", "syntax tree", "incremental parsing", "query"]
related: ["index.md", "api.md", "typed-api.md"]
---

# Tree-sitter getting started

Install the `treesitter` archive in `packages/` and import `treesitter`. Supply the absolute path to a compiled JSON grammar library as the program’s argument. The repository builder creates a JSON grammar fixture separately from the reusable runtime; the grammar is not implicitly bundled into every consumer.

This complete example loads that grammar, parses a JSON string, prints the root S-expression, and explicitly closes the node, tree, and parser in dependency order.

Grammar loading, ABI validation, allocation, use-after-close, and invalid coordinate operations can throw. Parse syntax errors instead remain in the returned tree; inspect `HasError` when accepting input. The example demonstrates API use and assumes its grammar path is valid.

```buoy
Import treesitter

Sub Main(args As Array Of String)
    If args.Count < 2 Then Throw New InvalidArgumentException("Pass a trusted JSON grammar library path")
    Dim language As New TSLanguage(args(1), "tree_sitter_json")
    Dim parser As New TSParser(language)
    Dim tree As TSTree = parser.Parse("{\"message\": \"Hello\"}")
    Dim root As TSNode = tree.RootNode()
    Print root.SExpression()
    root.Close()
    tree.Close()
    parser.Close()
End Sub
```

For incremental parsing, call `oldTree.Edit(...)` with coordinates matching the actual source change before `parser.ParseIncremental(newSource, oldTree)`. The wrapper checks numeric bounds and ordering but does not retain source text to verify those coordinates. An independent `tree.Copy()` is available before mutation. Ensure native runtime slices exist for each target OS/architecture; a successful compiler check alone does not provide missing native libraries.

## Continue reading

- [Overview](index.md)
- [Getting started](getting-started.md)
- [API reference](api.md)
