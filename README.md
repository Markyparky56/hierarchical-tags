# hierarchical-tags

**Very much WIP**

Hierarchical Tags in the style of Unreal Engine `FGameplayTag`. Built on top of a (hopefully) fast and cache-friendly `HashedString`/`HString`, similar to an `FName`.

Hashing courtesy of [xxHash](https://github.com/Cyan4973/xxHash), with an alternative to use [CityHas](https://github.com/Markyparky56/cityhash). Macros to choose hash size (64bit default) included. 

Current State:
- `HashedString` creation and addition to backend map
- String retrieval from `HashedString`
- `HashedStringMap` structure resembling a Hash Table, using the Hash from `HashedString` as keys
- String Utils to explode hierarchical strings (strings of the form `A.B.C`)

To-Do List:
- `HierarchicalTag`/`HTag` structure
- Comparison functions for `HashedString` and `HierarchicalTag`, case-sensitivity selectable
- Functions to check parent and child tags for `HierarchicalTag`
- Companion functions/structures for `HierarchicalTag` to facilitate retrieving parent tags efficiently
- Companion structure to hold multiple `HierarchicalTag`s 
- Option to override default tag-seperator
- Ability to load tags in bulk
- MORE & BETTER TESTS

Future-Concerns:
- Storing the Hash directly in `HashedString` could impact cache performance with 2 64bit hashes per structure. 
- `HashedStringMap` is not thread-safe in the slightest
- `FName`s support some form of "lexical" less-than/greater-than functions, I assume to allow for basic list sorting? Do we care about that?
- `FGameplayTag` can achieve efficient network transfer with "fast gameplay tag replication" because all tags are supposed to be known at start-up and therefore have some shared index on both client and server. The ability to block tags from being created at runtime could be useful in support of a similar system.
