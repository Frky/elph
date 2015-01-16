# elfdump

Tool for ELF64 binaries providing several functionalities, among which:

* display information about ELF header, section table, program table, symbol table, etc. (output is readelf-like)
* provide information about function location in binary 
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

### Basics 

Basic usage: `./elfdump [options] target_binary`
With no option, this will basically do nothing but parse `target_binary` and return.
Option `-q` (or `--quiet`) disable global information printing

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
will be executed, the program will run normally. If not specified, it is very likely that the program will crash after having executed the payload (depending on the 
patcing strategy and the payload).

#### Patching examples

* `./elfdump -o malicious -p payload/bash example/helloworld`: This will add a shellcode to `helloworld`, and so running this program will now result in
a bash opening. 
* `./elfdump -o harmless -p payload/nop -j example/helloworld`: This will add a payload consisting of four NOPs in the binary, and because `-j` was specified, 
after the NOPs the program will jump to the initial entry point, and then print "Hello world". Without `-j` option, the program crashes.


## Patching methods

In current version, two methods of patching are implemented. The method being used is decided regarding the size of the payload. In both cases, we use the fact that `NOTE` segment is almost always present in binaries, and almost always useless.

### In `NOTE` segment
If the target binary has a `NOTE` segment, and if this `NOTE` segment is large enough to embed the payload, then this method is choosen. Basically, it consists in:

* Changing type and flags of `NOTE` segment to support code execution
* Load payload in `NOTE` segment
* Modify the entry point to point into the beginning of the payload

### In a new segment
If it is not possible to incorporate the payload in `NOTE` segment, then a new segment is created at the end of the file. In order to preserve consistency of offsets in file, we cannot add an entry into program header table (which is at the beginning of the ELF file). So we overwrite the `NOTE` segment header, and replace it with a header corresponding to the new segment. Finally, we load the payload at the end of the file, and edit the entry point.  

## Current limitations

### Binary format
Currently, elfdump only supports 64 bits binary in little-endian mode.

### Patching
The current patching methods are, by design, very easy to detect. Thus, if the goal is to patch a binary remaining as discrete as possible, a lot of work still has to be done.


## External links
 * http://downloads.openwatcom.org/ftp/devel/docs/elf-64-gen.pdf
 * http://www.linuxsecurity.com/resource\_files/documentation/virus-writing-HOWTO/\_html/additional.cs.html 
 * http://shell-storm.org/shellcode/

 
