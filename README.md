# CMarshal

[Online Demo](https://cmarshal.herokuapp.com/)

CMarshal brings Go style JSON marshaling / unmarshaling to C.

You can generate cJSON based JSON serializers and deserializers by adding
```c
/* cmarshal:`true` */
```
to your C struct.

## Compile

You'll need `clang` and `libclang` installed.

You may also need to modify `Makefile` so `-I` points to the correct
include path for `libclang`

Do:

```bash
make cmarshal
```

This will compile cmarshal to the project directory.

## Usage

Say that you have a C header file that contains some structs

```c
typedef struct {
	int a;
	int b;
} MyStruct;
```

You can add some comments to it

```c
/*
--- Add a configuration comment:
--- Note that the semi-colon after the comment is significant.
cmarshal:`{
	"cJSONInclude": "./cJSON.h",
	"unmarshalerHeaderFile": "./demo.unmarshaler.h",
	"unmarshalerImplFile": "./demo.unmarshaler.c",
	"marshalerHeaderFile": "./demo.marshaler.h",
	"marshalerImplFile": "./demo.marshaler.c"
}`
*/;

/*
--- Annotate you struct with:
cmarshal:`true`
*/
typedef struct {
	int a;
	int b;
	int c;      /* cmarshal:`{"key": "d"}` */
	int ignore; /* cmarshal:`{"ignore": true}` */
} MyStruct;
```

Now `cd` to the header of the directory.
Assuming your file is called `demo.h`, do:

`path/to/cmarshal ./demo.h`

This will generate `./demo.marshaler.[ch]` and `./demo.unmarshaler.[ch]`

You will also need to include cJSON in your project.

You can then do things like

```c
MyStruct s = {1, 2, 3, 4};
cJSON *json = marshal_MyStruct(&s);
char *str = cJSON_Print(json);
printf("%s\n", str);
free(str);
cJSON_Delete(json);
```

Which will print

```json
{
	"a": 1,
	"b": 2,
	"d": 3
}
```

See the `demo` directory for a more in-depth example,
including how to unmarshal a JSON.

Also see `src/jsonstructs.h` for all available options.
(Fun fact: the `cmarshal:...` annotations are also parsed using
`cmarshal` generated unmarshalers. `cmarshal` itself is bootstrapped!)

## Use cases

* Auto-generated JSON config parsers
* Simple struct printer (as a debugging mechanism)
* Introspecting C structs using cJSON API
* Easily bloat your program line count!
  (not the intended use case, of course)

## Contributing
Pull requests are welcome.

## License
[MIT](https://choosealicense.com/licenses/mit/)
