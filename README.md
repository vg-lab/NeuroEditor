# NeuroEditor

Neuromorphological tracings editor

NeuroEditor is a software tool for the visualization and edition of morphological tracings. It offers manual edition capabilities together with a set of algorithms that can automatically identify potential errors in the tracings and, in some cases, propose a set of actions to correct them automatically.

NeuroEditor visualizes the original tracing, the modified tracing and a 3D mesh that approximates the neuronal membrane. This approximation of the cell's surface is computed on-the-fly and instantly reflects any changes made to the tracing.

Besides the methods already implemented, NeuroEditor can be easily extended by users, who can program their own algorithms in Python and run them within the tool.

## Building

```bash
git clone https://github.com/vg-lab/NeuroEditor.git
cd NeuroEditor

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

In-source builds are not allowed, so always configure into a separate build directory. If `CMAKE_BUILD_TYPE` is not specified, it defaults to `Debug`; use `Release` for the best performance.

## CMake options

| Option | Default | Description |
| ------ | ------- | ----------- |
| `BUILD_SHARED_LIBS` | `ON` | Build the libraries built along with NeuroEditor as shared libraries (set to `OFF` for static) |
| `BUILD_DOCS` | `ON` | Generate Doxygen documentation when Doxygen is found |

Example:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_DOCS=OFF
```

## Dependencies

NeuroEditor resolves its dependencies through CMake. Most of them are looked up with `find_package()` and must be installed on your system.

For some dependencies, CMake first looks for an installed copy with `find_package()`. Only if none is found is the dependency downloaded and built using `FetchContent`. This supports two workflows:

- System packages: install the dependencies with your package manager, or point CMake at them with `CMAKE_PREFIX_PATH`.
- Automatic fetching: on a system without them installed, the missing dependencies are fetched automatically during configuration.

## Installing

To install NeuroEditor:

```bash
cmake --install build --prefix /your/prefix
```

## Documentation

The API documentation is generated from the source with Doxygen. To build it locally (requires Doxygen and `BUILD_DOCS=ON`):

```bash
cmake --build build --target NeuroEditor_doxygen
```

The HTML output is written to `build/docs_doxygen/html`.

## License

NeuroEditor is released under the [GNU General Public License v3.0](LICENSE.txt).

Developed at the [Visualization and Graphics Lab (VG-Lab)](https://github.com/vg-lab).
