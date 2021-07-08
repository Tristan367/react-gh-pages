using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class ChunkWithMesh 
{
    public Mesh mesh;
    public int voxelIndex;

    public List<Vertex> vertices = new List<Vertex>();
    public List<int> indices = new List<int>();

    public Vector3Int position;

    public bool[] existingOrthogonalNeighbors = new bool[6];

    public bool empty = false;
    public bool fullButNoVertices = false;
}
