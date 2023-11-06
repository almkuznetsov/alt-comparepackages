# ComparePackages Library and CLI Utility
The ComparePackages library is designed to compare binary package lists from two different branches and generate a JSON report with the differences.

## Installation
Clone the repository:
```bash
git clone https://github.com/almkuznetsov/basealt_test
```
### Quick installation (build all)
Run buildall.sh script
```bash
./buildall.sh
```
If you want to build library and cli separately:
### Building the Library
Run buildlib.sh script
```bash
lib/buildlib.sh
```
According to FHS standard output file `libcomparepackages.so.1.0.0` should be stored at `/usr/local/lib`
### Building the CLI Utility
Run buildcli.sh script
```bash
./buildcli.sh
```

## Usage
### Library
To use the ComparePackages library in your C++ project, include the comparepackages.hpp header and link against the shared library.

The library contains 2 functions: 
1. `getPackages` allows you to download packages through API request to https://rdb.altlinux.org/api/
2. `comparePackages` allows you to compare two branches

### JSON Report
The JSON report generated by the utility will have the following structure:

```json
{
"arch_onlyBranch1package": [...],
"arch_onlyBranch2package": [...],
"arch_differentVersionPackages": [...]
}
```
- arch: architecture type (x86_64, i586, noarch, aarch64, armh, ppc64le, srpm).
- onlyBranch1package: Packages present in branch1 but not in branch2. 
- onlyBranch2package: Packages present in branch2 but not in branch1. 
- differentVersionPackages: Packages with version numbers greater in branch1 than in branch2.
### Example  usage
At the moment `getPackages` function is still in development mode, so first, you'll need some packages data to compare. Use 
```bash
./download.sh
```
to download 2 json files with branches data.

At the moment, json-files' names are hard-coded into the main.cpp file, but you can change them if you need to.
To run CLI Utility use
```bash
./build/main
```
