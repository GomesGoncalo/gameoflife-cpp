# genetic_algo - An executable

The `genetic_algo` executable is a <SUMMARY-OF-FUNCTIONALITY>.

Note that the `genetic_algo` executable in this package provides `build2` metadata.


## Usage

To start using `genetic_algo` in your project, add the following build-time
`depends` value to your `manifest`, adjusting the version constraint as
appropriate:

```
depends: * genetic_algo ^<VERSION>
```

Then import the executable in your `buildfile`:

```
import! [metadata] <TARGET> = genetic_algo%exe{<TARGET>}
```


## Importable targets

This package provides the following importable targets:

```
exe{<TARGET>}
```

<DESCRIPTION-OF-IMPORTABLE-TARGETS>


## Configuration variables

This package provides the following configuration variables:

```
[bool] config.genetic_algo.<VARIABLE> ?= false
```

<DESCRIPTION-OF-CONFIG-VARIABLES>
