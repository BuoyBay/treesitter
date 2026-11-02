# Parse and query syntax with incremental tree-sitter trees

A cross-platform `treesitter` package wrapping the native tree-sitter runtime.
It loads compiled grammars and exposes managed parsers, trees, nodes and structural
query captures. The runtime is embedded in the bridge; consumers do not need a
separate tree-sitter installation.

## Install

From a Buoy consumer project with BPM installed:

```sh
bpm add treesitter --git https://github.com/BuoyBay/treesitter.git --rev refs/heads/main
```basic

BPM records the resolved commit in its lockfile. Keep that lockfile with the
consumer project for reproducible installs.

## Example

```basic
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

## Build and API reference

Source is in `modules/`, with native/runtime inputs where required. Use the
[package guide](docs/API.md) for backend-specific build prerequisites, ownership
rules and supported targets. Rebuilding a native backend requires that target's
compiler and SDK; sealing an archive alone does not rebuild the backend.

## Local Buoy setup

On your own machine, install Buoy and run:

```sh
buoy ai init --both
```

## Contributing

[Contributing](CONTRIBUTING.md) · [License](LICENSE)
