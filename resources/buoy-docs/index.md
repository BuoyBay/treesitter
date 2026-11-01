---
title: "Tree-sitter"
summary: "Managed native syntax parsers, immutable tree snapshots, node navigation, incremental edits, and structural captures."
category: "Overview"
order: 0
keywords: ["tree-sitter", "parser", "syntax tree", "incremental parsing", "query"]
related: ["getting-started.md", "api.md", "typed-api.md"]
---

# Tree-sitter

The `treesitter` package wraps the native tree-sitter runtime with managed Buoy parsers, trees, nodes, queries, and capture cursors. The runtime is embedded in the package bridge; grammars are separate trusted native libraries.

A grammar library exports a factory such as `tree_sitter_json`. Its ABI must be accepted by the bundled runtime. Load only trusted libraries, normally by absolute path: grammar loading executes native code. The wrapper loads compiled grammars; it does not generate a Buoy grammar.

## Coordinates and ownership

All source offsets, point columns, and edit coordinates are UTF-8 bytes. Rows are zero-based and end positions are exclusive. Parse errors remain inspectable through tree/node error state rather than automatically causing a parse exception.

ARC retains native dependencies. Nodes retain their tree; trees and queries retain their language; capture cursors retain their query and root node. Explicit Close methods are idempotent, but closing or editing a tree invalidates its existing nodes/cursors. Confine an object graph to one thread or synchronize it externally.

## Continue reading

- [Overview](index.md)
- [Getting started](getting-started.md)
- [API reference](api.md)
