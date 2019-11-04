# spacecurvetransformer
Transformation of Triangular-Meshes along Spacecurves

### About
This is the prototype implementation for the Bachelorthesis


**Transformation von Dreiecksnetzen entlang von Raumkurven**

This implementation is written in C++ and needs the GLM Library to work properly.
it can be build via make:
``` sh
> make
```
It can be run via:
``` sh
> ./main.out [object] [curve] [outfile]
```
**[object]** the object which should be transformed. (.obj)

**[curve]** the curve along which the transformation should take place. (.csv)

**[outfile]** the outfile where the results should be saved.  (.obj)

Beispielausführung: 
``` sh
./main.out objects/bunny_simple.obj curves/loopy.csv results/bunnyXloopy.obj
```
