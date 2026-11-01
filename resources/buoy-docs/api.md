---
title: "Tree-sitter API"
summary: "Public APIs and lifecycle contracts for Tree-sitter."
category: "Reference"
order: 20
keywords: ["tree-sitter", "parser", "syntax tree", "incremental parsing", "query"]
symbols: ["TSLanguage", "TSParser", "TSParser.Parse", "TSParser.ParseIncremental", "TSTree", "TSTree.RootNode", "TSTree.Edit", "TSTree.Copy", "TSNode", "TSNode.Child", "TSNode.ChildByField", "TSNode.SExpression", "TSQuery", "TSQuery.Captures", "TSCaptureCursor", "TSCaptureCursor.MoveNext"]
related: ["index.md", "getting-started.md", "typed-api.md"]
---

# Tree-sitter API

| Type | Main operations |
| --- | --- |
| `TSLanguage(libraryPath, symbol)` | Load a grammar factory; ARC-only lifetime protects dependents. |
| `TSParser(language)` | `Parse`, `ParseIncremental`, and idempotent `Close`. Closing the parser leaves returned trees usable. |
| `TSTree` | `RootNode`, independent `Copy`, coordinate-based `Edit`, and `Close`. |
| `TSNode` | `Child(index, named = False)`, `Parent`, `ChildByField`, `SExpression`, and `Close`. Absent relatives return `Nil`. |
| `TSQuery(language, source)` | Compile structural capture patterns; ARC retains the grammar. |
| `TSCaptureCursor` | `MoveNext`, current `Name`, `PatternIndex`, `Node()`, and `Close`. |

## Node information

`TypeName`, `StartByte`, `EndByte`, `StartRow`, `StartColumn`, `EndRow`, and `EndColumn` describe a node’s type and byte span. `ChildCount`, `NamedChildCount`, and `IsNamed` support navigation. `HasError` and `IsMissing` expose parse recovery state. Child indices are zero-based.

## Incremental edits

`TSTree.Edit(startByte, oldEndByte, newEndByte, startRow, startColumn, oldEndRow, oldEndColumn, newEndRow, newEndColumn)` updates the old tree before reuse. All offsets/columns use UTF-8 bytes and must fit uint32 bounds. Editing a tree increments its generation and invalidates previous nodes/cursors. `ParseIncremental` requires a tree associated with the same language instance.

## Query scope and lifecycle

`TSQuery.Captures(node)` returns a cursor. Call `MoveNext()` before consuming its current capture, and release cursors/nodes before closing or editing their tree. Keep the supplied node open until its cursor is closed: explicitly calling that node’s `Close()` invalidates the cursor, even if the tree remains open. Closing the cursor leaves its supplied node and previously returned capture nodes usable; closing the supplied node also leaves previously returned capture nodes usable, provided their tree remains open and unedited. Query predicates and directives such as `#eq?`, `#match?`, and `#set!` reject with error 100 rather than silently running without evaluation. Native query errors include a byte offset and error code.

This release does not expose streaming input, included ranges, cancellation, query match limits, changed ranges, or tree-walking cursors. Query iteration can be expensive for large input or untrusted patterns. The package’s compiler target branches and shipped native runtime slices are separate compatibility facts.

## Continue reading

- [Overview](index.md)
- [Getting started](getting-started.md)
- [API reference](api.md)


[Full typed API: properties, parameters, defaults, and returns](typed-api.md)
