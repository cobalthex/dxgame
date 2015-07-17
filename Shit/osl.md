# Object Serialization Language (OSL) #
---
OSL is a flexible data description language aimed at providing a convenient method for textual object representation. This language is in the same family as JSON, OpenDDL, and Yaml (among others).

## Schema ##

OSL is an object based format. Documents (Single files) contain a list of objects (usually mapped by name). Each object contains any number of properties, each with a variable list of values.

E.g.: `property: value1 value2 value3`

OSL supports a number of types for each value:

1. Null
+ Boolean
+ Integer
+ Decimal
+ String
+ Date
+ Time
+ Object
+ Object Reference
+ Type

***All values are case insensitive, however object references and strings are case sensitive***  
***All indexes start at 0***

---

### Null: ###
Null values can be read in as either `null` or `nil`.

### Boolean: ###
Boolean values can be read in as either `true` or `false`.

### Integer: ###
Integer values are whole numbers, either negative or positive. They may use (base 10) exponents, e.g. `12E4 (120000)`. The underlying storage must have at least 32 bits of precision.

### Decimal: ###
Decimal values are decimal numbers, either negative or positive. They may use (base 10) exponents, e.g.: `12.4e3 (12400)`. The underlying storage must have at least 32 bits of precision.

### String: ###
String values contain a string of characters. Ideally the string should be UTF-8 but this is implementation dependent. Strings are enclosed with either a pair of double quotes `"..."` or single quotes `'...'`. In addition, single word values with no quotes that do not describe any of the other types are automatically stored as strings, e.g. `test` becomes `"test"`.

### Date: ###
Dates follow the `year-month-date` format. The year must be able to store at least four digits and two digits for the month and day. There is no requirement for the month and day to stay within Gregorian limits (This is implementation defined).

### Time: ###
Times follow the `hours:minutes:seconds.milliseconds` format. Hours may be as long as desired, however, minutes and seconds are both capped at 60. Milliseconds is optional and is clipped at 1000. Minutes and seconds must have a leading zero (if necessary), and milliseconds must have up to two leading zeros (if necessary).

### Array: ###
Arrays are linear collections of any of OSL's supported types. Arrays are always wrapped in square brackets (`[ ... ]`). 

### Object: ###
Objects are the core container of OSL. Objects each have a name, along with an optional type and attributes. Objects contain named properties, each with a variable [indexable] list of values (called the value-list). There can be an infinite number of unique properties with an infinite number of values.
The syntax for objects is as follows:

	[Type] <Name > [: [attribute1] [attribute2] [...]] {
	  <Property name >: <arg1> [[arg2] [...]];
	  ...
	}

*Square brackets `[]` denote optional parameters. Angle brackets `<>` are required*  
*(All items with a space at the end, such as `<Name >` are not word terminated)*

**Semicolons are required at the end of each property value-list. The final semicolon is optional.**

Inline objects do not have a type, name, or attributes (but can be emulated with the value-list)  
Types have no inherent meaning

### Object Reference: ###
todo

### Type: ###
todo

---
OSL documents are an indexable list of OSL objects. They may not have any other type in the base of the document.  
OSL can be validated by a schema but a schema is not required for use.