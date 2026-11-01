---
title: "treesitter typed API"
summary: "Source-derived public signatures, properties, parameters, defaults, and return values."
category: "Reference"
order: 21
types: ["TSCaptureCursor", "TSLanguage", "TSNode", "TSParser", "TSQuery", "TSTree"]
dependencies: []
symbols: ["TSCaptureCursor", "TSLanguage", "TSNode", "TSParser", "TSQuery", "TSTree"]
related: ["api.md", "getting-started.md", "index.md"]
---

# treesitter typed API

Generated from the package source declarations. Platform guards are shown where present; inherited members are documented on the base type. See the [API contracts](api.md) for behavior and limitations.

## TSLanguage

`Public Class TSLanguage`

### Constructor

`Public Sub Constructor(libraryPath As String, symbol As String)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `libraryPath` | `String` | `ByVal` | Required |
| `symbol` | `String` | `ByVal` | Required |

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

## TSParser

`Public Class TSParser`

### Constructor

`Public Sub Constructor(language As TSLanguage)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `language` | `TSLanguage` | `ByVal` | Required |

Returns: no value.

### Close

`Public Sub Close()`

No parameters.

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

### Parse

`Public Function Parse(source As String) As TSTree`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `source` | `String` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `TSTree` |

### ParseIncremental

`Public Function ParseIncremental(source As String, oldTree As TSTree) As TSTree`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `source` | `String` | `ByVal` | Required |
| `oldTree` | `TSTree` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `TSTree` |

## TSTree

`Public Class TSTree`

### Close

`Public Sub Close()`

No parameters.

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

### Copy

`Public Function Copy() As TSTree`

No parameters.

#### Returns

| Type |
| --- |
| `TSTree` |

### RootNode

`Public Function RootNode() As TSNode`

No parameters.

#### Returns

| Type |
| --- |
| `TSNode` |

### Edit

`Public Sub Edit(startByte As Integer, oldEndByte As Integer, newEndByte As Integer, startRow As Integer, startColumn As Integer, oldEndRow As Integer, oldEndColumn As Integer, newEndRow As Integer, newEndColumn As Integer)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `startByte` | `Integer` | `ByVal` | Required |
| `oldEndByte` | `Integer` | `ByVal` | Required |
| `newEndByte` | `Integer` | `ByVal` | Required |
| `startRow` | `Integer` | `ByVal` | Required |
| `startColumn` | `Integer` | `ByVal` | Required |
| `oldEndRow` | `Integer` | `ByVal` | Required |
| `oldEndColumn` | `Integer` | `ByVal` | Required |
| `newEndRow` | `Integer` | `ByVal` | Required |
| `newEndColumn` | `Integer` | `ByVal` | Required |

Returns: no value.

## TSNode

`Public Class TSNode`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `TypeName` | `String` | Yes | No | — | — |
| `StartByte` | `Integer` | Yes | No | — | — |
| `EndByte` | `Integer` | Yes | No | — | — |
| `StartRow` | `Integer` | Yes | No | — | — |
| `StartColumn` | `Integer` | Yes | No | — | — |
| `EndRow` | `Integer` | Yes | No | — | — |
| `EndColumn` | `Integer` | Yes | No | — | — |
| `ChildCount` | `Integer` | Yes | No | — | — |
| `NamedChildCount` | `Integer` | Yes | No | — | — |
| `IsNamed` | `Boolean` | Yes | No | — | — |
| `HasError` | `Boolean` | Yes | No | — | — |
| `IsMissing` | `Boolean` | Yes | No | — | — |

### Close

`Public Sub Close()`

No parameters.

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

### Child

`Public Function Child(index As Integer, named As Boolean = False) As TSNode`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `index` | `Integer` | `ByVal` | Required |
| `named` | `Boolean` | `ByVal` | `False` |

#### Returns

| Type |
| --- |
| `TSNode` |

### Parent

`Public Function Parent() As TSNode`

No parameters.

#### Returns

| Type |
| --- |
| `TSNode` |

### ChildByField

`Public Function ChildByField(name As String) As TSNode`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `name` | `String` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `TSNode` |

### SExpression

`Public Function SExpression() As String`

No parameters.

#### Returns

| Type |
| --- |
| `String` |

## TSQuery

`Public Class TSQuery`

### Constructor

`Public Sub Constructor(language As TSLanguage, source As String)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `language` | `TSLanguage` | `ByVal` | Required |
| `source` | `String` | `ByVal` | Required |

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

### Captures

`Public Function Captures(node As TSNode) As TSCaptureCursor`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `node` | `TSNode` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `TSCaptureCursor` |

## TSCaptureCursor

`Public Class TSCaptureCursor`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `Name` | `String` | Yes | No | — | — |
| `PatternIndex` | `Integer` | Yes | No | — | — |

### Close

`Public Sub Close()`

No parameters.

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

### MoveNext

`Public Function MoveNext() As Boolean`

No parameters.

#### Returns

| Type |
| --- |
| `Boolean` |

### Node

`Public Function Node() As TSNode`

No parameters.

#### Returns

| Type |
| --- |
| `TSNode` |
