using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Chunk
{
    public Mesh mesh;
    public char[] voxels;
    public int chunkRowSize;
    public int chunkPlaneSize;
    public int chunkSize;
    public Vector3Int position;
    public ChunksManager chunksManager;

    Vector3[] vertexBuffer; 
    int vertexCounter = 0;
    int[] triangleBuffer;
    int triangleCounter = 0;
    Vector3[] normalsBuffer;

    Vector3[] verts;
    int[] tris;
    Vector3[] norms;

    public bool ready = false;
    public bool empty = true;

    public char[][] neighboringChunkVoxels;


    public bool[] existingOrthogonalNeighbors = {
        false, // left
        false, // right
        false, // back
        false, // front
        false, // bottom
        false // top
    };

    


    public void InitializeChunk()
    {
        vertexBuffer = new Vector3[chunkSize * 24];
        normalsBuffer = new Vector3[chunkSize * 24];
        triangleBuffer = new int[chunkSize * 36];
    }

    public void CalculateMeshData() // done on another thread
    {
        vertexCounter = 0;
        triangleCounter = 0;
        Vector3Int offset = new Vector3Int(0, 0, 0);

        for (int i = 0; i < chunkSize; i++) // for each voxel
        {
            if (empty)
            {
                break;
            }
            if (voxels[i] != 0)
            {
                int leftFaceState = 0;
                int rightFaceState = 0;
                int backFaceState = 0;
                int frontFaceState = 0;
                int bottomFaceState = 0;
                int topFaceState = 0;

                // left
                if (i % chunkRowSize == 0) // if we are at the edge of a chunk
                {
                    if (existingOrthogonalNeighbors[0])
                    {
                        if (neighboringChunkVoxels[0][i + (chunkRowSize - 1)] == 0)
                        {
                            leftFaceState = 1;
                        }
                    }

                }
                else
                {
                    if (voxels[i - 1] == 0)
                    {
                        leftFaceState = 1;
                    }
                }
                // right
                if (i % chunkRowSize == chunkRowSize - 1)
                {
                    if (existingOrthogonalNeighbors[1])
                    {
                        if (neighboringChunkVoxels[1][i - (chunkRowSize - 1)] == 0)
                        {
                            rightFaceState = 1;
                        }
                    }
                }
                else
                {
                    if (voxels[i + 1] == 0)
                    {
                        rightFaceState = 1;
                    }
                }
                // back
                if ((i % chunkPlaneSize) - chunkRowSize < 0)
                {
                    if (existingOrthogonalNeighbors[2])
                    {
                        if (neighboringChunkVoxels[2][i + (chunkPlaneSize - chunkRowSize)] == 0)
                        {
                            backFaceState = 1;
                        }
                    }
                }
                else
                {
                    if (voxels[i - chunkRowSize] == 0)
                    {
                        backFaceState = 1;
                    }
                }
                // front
                if ((i % chunkPlaneSize) + chunkRowSize >= chunkPlaneSize)
                {
                    if (existingOrthogonalNeighbors[3])
                    {
                        if (neighboringChunkVoxels[3][i - (chunkPlaneSize - chunkRowSize)] == 0)
                        {
                            frontFaceState = 1;
                        }
                    }
                }
                else
                {
                    if (voxels[i + chunkRowSize] == 0)
                    {
                        frontFaceState = 1;
                    }
                }
                // bottom
                if (i - chunkPlaneSize < 0)
                {
                    if (existingOrthogonalNeighbors[4])
                    {
                        if (neighboringChunkVoxels[4][i + (chunkSize - chunkPlaneSize)] == 0)
                        {
                            bottomFaceState = 1;
                        }
                    }
                }
                else
                {
                    if (voxels[i - chunkPlaneSize] == 0)
                    {
                        bottomFaceState = 1;
                    }
                }
                // top
                if (i + chunkPlaneSize >= chunkSize)
                {
                    if (existingOrthogonalNeighbors[5])
                    {
                        if (neighboringChunkVoxels[5][i - (chunkSize - chunkPlaneSize)] == 0)
                        {
                            topFaceState = 1;
                        }
                    }
                }
                else
                {
                    if (voxels[i + chunkPlaneSize] == 0)
                    {
                        topFaceState = 1;
                    }
                }




                if (leftFaceState != 0)
                {
                    for (int f = 0; f < 6; f++)
                    {
                        triangleBuffer[triangleCounter++] = chunksManager.triangle4VertFacePattern[f] + vertexCounter;
                    }
                    for (int f = 0; f < 4; f++)
                    {
                        vertexBuffer[vertexCounter] = chunksManager.leftFace4Verts[f] + offset;
                        normalsBuffer[vertexCounter] = new Vector3(-1, 0, 0);
                        vertexCounter++;
                    }
                }
                if (rightFaceState != 0)
                {
                    for (int f = 0; f < 6; f++)
                    {
                        triangleBuffer[triangleCounter++] = chunksManager.triangle4VertFacePattern[f] + vertexCounter;
                    }
                    for (int f = 0; f < 4; f++)
                    {
                        vertexBuffer[vertexCounter] = chunksManager.rightFace4Verts[f] + offset;
                        normalsBuffer[vertexCounter] = new Vector3(1, 0, 0);
                        vertexCounter++;
                    }
                }
                if (backFaceState != 0)
                {
                    for (int f = 0; f < 6; f++)
                    {
                        triangleBuffer[triangleCounter++] = chunksManager.triangle4VertFacePattern[f] + vertexCounter;
                    }
                    for (int f = 0; f < 4; f++)
                    {
                        vertexBuffer[vertexCounter] = chunksManager.backFace4Verts[f] + offset;
                        normalsBuffer[vertexCounter] = new Vector3(0, 0, -1);
                        vertexCounter++;
                    }
                }
                if (frontFaceState != 0)
                {
                    for (int f = 0; f < 6; f++)
                    {
                        triangleBuffer[triangleCounter++] = chunksManager.triangle4VertFacePattern[f] + vertexCounter;
                    }
                    for (int f = 0; f < 4; f++)
                    {
                        vertexBuffer[vertexCounter] = chunksManager.frontFace4Verts[f] + offset;
                        normalsBuffer[vertexCounter] = new Vector3(0, 0, 1);
                        vertexCounter++;
                    }
                }
                if (bottomFaceState != 0)
                {
                    for (int f = 0; f < 6; f++)
                    {
                        triangleBuffer[triangleCounter++] = chunksManager.triangle4VertFacePattern[f] + vertexCounter;
                    }
                    for (int f = 0; f < 4; f++)
                    {
                        vertexBuffer[vertexCounter] = chunksManager.bottomFace4Verts[f] + offset;
                        normalsBuffer[vertexCounter] = new Vector3(0, -1, 0);
                        vertexCounter++;
                    }
                }
                if (topFaceState != 0)
                {
                    for (int f = 0; f < 6; f++)
                    {
                        triangleBuffer[triangleCounter++] = chunksManager.triangle4VertFacePattern[f] + vertexCounter;
                    }
                    for (int f = 0; f < 4; f++)
                    {
                        vertexBuffer[vertexCounter] = chunksManager.topFace4Verts[f] + offset;
                        normalsBuffer[vertexCounter] = new Vector3(0, 1, 0);
                        vertexCounter++;
                    } 
                }
            }

            offset.x++;
            if (offset.x >= chunkRowSize)
            {
                offset.x = 0;
                offset.z++;
                if (offset.z >= chunkRowSize)
                {
                    offset.z = 0;
                    offset.y++;
                }
            }
        }

        if (vertexCounter == 0)
        {
            empty = true;
        }

        verts = new Vector3[vertexCounter];
        norms = new Vector3[vertexCounter];
        for (int i = 0; i < vertexCounter; i++)
        {
            verts[i] = vertexBuffer[i];
            norms[i] = normalsBuffer[i];
        }
        tris = new int[triangleCounter];
        for (int i = 0; i < triangleCounter; i++)
        {
            tris[i] = triangleBuffer[i];
        }
    }

    public void RecalculateMeshIfAllNeighborsExist()
    {
        if (existingOrthogonalNeighbors[0] &&
            existingOrthogonalNeighbors[1] &&
            existingOrthogonalNeighbors[2] &&
            existingOrthogonalNeighbors[3] &&
            existingOrthogonalNeighbors[4] &&
            existingOrthogonalNeighbors[5])
        {
            empty = false;
            CalculateMeshData();
            ready = false;
        }
    }

    public void SetMeshData() // done on the main thread
    {
        mesh = new Mesh();
        mesh.Clear();
        mesh.MarkDynamic();
        mesh.SetVertices(verts);
        mesh.SetTriangles(tris, 0);
        mesh.SetNormals(norms);

        mesh.Optimize();
    }

}
