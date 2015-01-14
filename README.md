# elfdump

Tool for ELF64 binaries providing several functionalities, among which:

* display information about ELF header, section table, programme table, symbol table, etc. (output in readelf-like)
* provide information about function location in binary (stripped only for now)
* allow to patch a binary to add code (see limitations in corresponding	section)


## Examples
* `./elfdump -h example/helloworld`
* `./elfdump -p payload/bash -o infected_binary elfdump`


## Practical information
### Requirements
Because all the information needed are parsed by elfdump with no use of external tool (such as readelf), there is no requirement, except a c compiler and a linux system.


### Installation
This time I made a Makefile, so you can just use it.
`make`

## Usage

Basic usage: `./elfdump [options] target_binary`
With no option, this will basically do nothing but parse `target_binary` and return.

### Print information about a binary
The following options are available and allow to display information about the binary given as last parameter:

* `-h`: Binary header information (such as binary format, architecture, number of sections, etc.) 
* `-S`: Section headers information 
* `-s`: Display symbol table
* `-l`: Program headers information

### Retrieve functions
One of elfdump aims is to locate embedded functions of a binary. For now, the function detection is trivial and is only based on symbol table information. 
Of course, this is not possible for a stripped binary, so one of the next steps is to implement some heuristical function searching algorithms.

* `-f`: Print infomation about functions found in symbol table (such as name, offset in file)

### Patch a binary
The patch of a binary is the other main aim of elfdump. From a x86_64 payload (see `payload/` for examples), elfdump includes it in the target binary, 
and modify the entry point of the program to execute the payload. Please note that in no way the initial binary is modified ; a new file is generated.

* `-p path/to/payload`: This option gives the path to the payload to include into target binary. As soon as `-p` is specified, elfdump will generate
a new binary that includes this payload.
* `-o path/to/generated/binary`: Gives the path to the modified binary. If unset, then the generated binary is `./patched`
* `-j`: If this option is specified, the payload will be completed with a jump to the initial entry code of the binary. This means that after the payload
will be executed, the program will run normally. If not specified, it is very likely that the program will crash after having executed the payload.

#### Patching examples

* `./elfdump -o malicious -p payload/bash example/helloworld`: This will add a shellcode to `helloworld`, and so running this program will now result in
a bash opening. 
* `./elfdump -o harmless -p payload/nop -j example/helloworld`: This will add a payload consisting of four NOPs in the binary, and because `-j` was specified, 
after the NOPs the program will jump to the initial entry point, and then print "Hello world". Without `-j` option, the program crashes.


## Current limitations

### Binary format
Currently, elfdump only supports 64 bits binary in little-endian mode.

### Patching
For now, the patch is written in the note segment. The resize of this section has not been implemented yet, so the payload size is limited to the size of the note 
segment in the target binary file.
