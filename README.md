# spacecurvetransformer
Transformation of Triangular-Meshes along Spacecurves

### About
This is the improved implementation for the Bachelorthesis

**Transformation von Dreiecksnetzen entlang von Raumkurven**

the original Implementation at the time of the Bachelorthesis can be found under:

[Original Implementation](https://github.com/nicklasDohrn/spacecurvetransformer/tree/original)

Please note, that this Version is far superior in usability and functional depth, so i highly recommend using the current improved version.

This implementation is written in C++ and needs the GLM Library to work properly.
it can be build via make:
``` sh
> make
```
It can be run via:
``` sh
> ./main.out [object] [curve]
```
**[object]** the object which should be transformed. (.obj)

**[curve]** the curve along which the transformation should take place. (.csv)

Sample run: 
``` sh
./main.out objects/bunny_simple.obj curves/loopy.csv
```

see the -h Option to see more configuration options

The final version is also able to report all points that intersect with a given Lightroom Profile.

This can be called by simply providing a lightroom profile as 3rd argument:

```
./main.out objects/bunny_simple.obj curves/loopy.csv lightprofile/profile.obj
```

you can also use flags to further customize this mode.
