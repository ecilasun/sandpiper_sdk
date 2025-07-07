# BVH Tool

A tool that will process input geometry and produce a BVH structure using LBVH method.
You can read more on this subject here:
https://research.nvidia.com/publication/2012-06_maximizing-parallelism-construction-bvhs-octrees-and-k-d-trees

## Summary

This tool will read a list of raw triangles and export an LBVH structure, and also preview it.
It can run on Windows or Linux and preview the generated geometry using raytracing.

## Building and running on Windows

- Install 'visual studio code'.
- Open the root folder with bhv8-tool in vscode
- Switch debug configuration in vscode to '(Windows) Launch'
- Use 'configure' from build menu (ctrl+shift+b) (only needed once)
- Use 'build' from build menu
- Run the executable by hitting F5

## Building and running on Linux

- Install 'visual studio code', and in a terminal window run the following:
    ```
    sudo apt update
    sudo apt upgrade
    sudo apt install libx11-dev libsdl2-dev gdb
    ```
- Open the root folder with bhv8-tool in vscode
- Switch debug configuration in vscode to '(gbd) Launch'
- Use 'configure' from build menu (ctrl+shift+b) (only needed once)
- Use 'build' from build menu
- Run the executable by hitting F5

## Test output

If everything went well, you should see something similar to the following in the terminal window:
```
- find materials in: testscene/testscene.mtl

- Suzanne       | vertices > 507| texcoo| texcoords > 556 > 0   | normals > 1114| triang| triangles > 0 | material: None
- Plane | vertices > 511        | texcoords > 560       | normals > 1115        | triangles > 0 | material: None
- Cube  | vertices > 519        | texcoords > 574       | normals > 1121        | triangles > 0 | material: None
- Text  | vertices > 1135       | texcoords > 1190      | normals > 1132        | triangles > 0 | material: None
- Cone  | vertices > 1168       | texcoords > 1255      | normals > 1165        | triangles > 0 | material: None
LBVH data generated. Leaf node count:1653
```

After a short while (on first run) the following scene should be displayed in a window.

![Sample Output](images/output.png)
