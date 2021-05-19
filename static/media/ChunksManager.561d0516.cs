using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
using Unity.Collections;
using System.Threading;

public struct Vertex
{
    public Vector3 pos;
    public Color32 tint;
    public Vector2 textureCoords;

    public Vertex(Vector3 position, Color32 color, Vector2 textureCoordinates)
    {
        pos = position;
        tint = color;
        textureCoords = textureCoordinates;
    }
};

public class ChunksManager : MonoBehaviour
{

    const int chunkRowSize = 16;
    const int chunkPlaneSize = chunkRowSize * chunkRowSize;
    const int chunkSize = chunkRowSize * chunkPlaneSize;
    const int chunkSizeDiv4 = chunkSize / 4;

    Vector3Int[] voxelDataChunkOffsets;
    Vector3Int largestStructureOctantDimensions = new Vector3Int(2, 2, 2); // in chunks
    int largestStructureLargestDimension = 2;
    int surroundingVoxelDataSize;
    Vector3Int surroundingVoxelDataOctantDimensions;
    float dataUnloadDistance;
    Vector3Int[] renderChunkOffsets;
    int renderedChunkOffsetsSize;

    const int chunkRenderDistance = 16;
    List<Vector3Int> chunksInFrustum = new List<Vector3Int>();
    float unloadDistance;
    const int batchsize = 25;


    List<Vector3Int> newChunksForSurroundingChunks = new List<Vector3Int>();
    int newChunksForSurroundingChunksCount = 0;
    Dictionary<Vector3Int, int> surroundingChunksDataDictionary = new Dictionary<Vector3Int, int>(); // the chunk's position and its start index in the voxel buffer
    char[] voxelBuffer; // where all the currently loaded voxels are kept
    int voxelBufferChunkCount = 0;
    List<int> availablePositionsInVoxelBufferFromRemovals = new List<int>();
    uint maxChunks;

    List<Vector3Int> chunksToRender = new List<Vector3Int>();
    HashSet<Vector3Int> chunksHashSet = new HashSet<Vector3Int>();
    List<Vector3Int> closestChunksToRender = new List<Vector3Int>();

    List<Vector3Int> chunksToRecalculate = new List<Vector3Int>();
    Vector3Int[] chunkNeighborOperations;

    List<ChunkWithMesh> chunkList = new List<ChunkWithMesh>(); // used on the main thread for rendering
    List<ChunkWithMesh> chunksReadyForMeshSet = new List<ChunkWithMesh>(); 
    List<ChunkWithMesh> finishedChunks = new List<ChunkWithMesh>(); 

    List<ChunkWithMesh> chunksToRemove = new List<ChunkWithMesh>();
    List<Vector3Int> surroundingChunksDictionaryChunksToRemove = new List<Vector3Int>();




    Vector3Int[] leftFace4Verts = new Vector3Int[]
        {
            new Vector3Int(0,0,1),
            new Vector3Int(0,1,1),
            new Vector3Int(0,1,0),
            new Vector3Int(0,0,0),
        };
    Vector3Int[] rightFace4Verts = new Vector3Int[]
        {
            new Vector3Int(1,0,0),
            new Vector3Int(1,1,0),
            new Vector3Int(1,1,1),
            new Vector3Int(1,0,1),
        };
    Vector3Int[] backFace4Verts = new Vector3Int[]
        {
            new Vector3Int(0,0,0),
            new Vector3Int(0,1,0),
            new Vector3Int(1,1,0),
            new Vector3Int(1,0,0),
        };
    Vector3Int[] frontFace4Verts = new Vector3Int[]
        {
            new Vector3Int(1,0,1),
            new Vector3Int(1,1,1),
            new Vector3Int(0,1,1),
            new Vector3Int(0,0,1),
        };
    Vector3Int[] bottomFace4Verts = new Vector3Int[]
        {
            new Vector3Int(1,0,0),
            new Vector3Int(1,0,1),
            new Vector3Int(0,0,1),
            new Vector3Int(0,0,0),
        };
    Vector3Int[] topFace4Verts = new Vector3Int[]
        {
            new Vector3Int(0,1,0),
            new Vector3Int(0,1,1),
            new Vector3Int(1,1,1),
            new Vector3Int(1,1,0),
        };
    int[] triangle4VertFacePattern = new int[]
        {
            0,1,2, 0,2,3
        };
    Vector2[] textureOffsets;


    public GameObject player;
    public GameObject playerCamera;
    Vector3Int playerPos; // for multi threading, the chunk coordinate the player is in
    Vector3 playerCameraPos;
    Vector3Int playerChunk;
    Vector3Int playerChunkHolderThread;
    Vector3 cameraRight, cameraUp, cameraForward;
    Vector3Int[] cornersOffsets =
    {
        new Vector3Int(0, 0, 0),
        new Vector3Int(chunkRowSize, 0, 0),
        new Vector3Int(0, chunkRowSize, 0),
        new Vector3Int(0, 0, chunkRowSize),

        new Vector3Int(chunkRowSize, chunkRowSize, 0),
        new Vector3Int(chunkRowSize, 0, chunkRowSize),
        new Vector3Int(chunkRowSize, chunkRowSize, 0),
        new Vector3Int(chunkRowSize, chunkRowSize, chunkRowSize)
    };

    struct TristanPlane
    {
        public Vector3 a, b, c, n;
    }
    TristanPlane[] frustumTristanPlanes = new TristanPlane[4];


    public ComputeShader computeNoiseShader;
    ComputeBuffer chunkPositionsBuffer;
    Vector3Int[] chunkPositionsToSendToGPU;
    ComputeBuffer voxelDataReturnBuffer;
    int[] voxelDataBufferCPU;
    const int gridXSize = 256;

    public Material material;


    Thread calculationThread;
    bool generatingChunksDone = false;
    bool computeSurroundings = false;
    bool surroundingsJustComputed = false;
    bool runThread = true;
    float time;


    void Start()
    {
        textureOffsets = new Vector2[] // needs to be done in start for some reason
        {
        new Vector2(0.0f,0.0f),
        new Vector2(0.0f,1.0f),
        new Vector2(1.0f,1.0f),
        new Vector2(1.0f,0.0f)
        };

        unloadDistance = chunkRenderDistance * chunkRowSize;
        dataUnloadDistance = (chunkRenderDistance + largestStructureLargestDimension) * chunkRowSize * 1.733f;
        surroundingVoxelDataOctantDimensions = new Vector3Int(chunkRenderDistance + largestStructureOctantDimensions.x, chunkRenderDistance + largestStructureOctantDimensions.y, chunkRenderDistance + largestStructureOctantDimensions.z);
        surroundingVoxelDataSize = surroundingVoxelDataOctantDimensions.x * surroundingVoxelDataOctantDimensions.y * surroundingVoxelDataOctantDimensions.z * 8;
        CreateChunkOffsetsForSurroundingVoxels();
        renderedChunkOffsetsSize = (int)Mathf.Pow(chunkRenderDistance * 2, 3);
        CreateChunkOffsets();
        chunkNeighborOperations = new Vector3Int[]
        {
            new Vector3Int(-chunkRowSize, 0, 0),
            new Vector3Int(chunkRowSize, 0, 0),
            new Vector3Int(0, 0, -chunkRowSize),
            new Vector3Int(0, 0, chunkRowSize),
            new Vector3Int(0, -chunkRowSize, 0),
            new Vector3Int(0, chunkRowSize, 0),
        };


        maxChunks = (uint)Mathf.Pow((chunkRenderDistance + largestStructureLargestDimension) * 2, 3) * 2; // times 2 just to give a little extra room
        voxelBuffer = new char[chunkSize * maxChunks];



        time = Time.deltaTime;
        playerPos = Vector3Int.CeilToInt(player.transform.position); // for the threads to reference
        playerCameraPos = playerCamera.transform.position;
        cameraRight = playerCamera.transform.right;
        cameraUp = playerCamera.transform.up;
        cameraForward = playerCamera.transform.forward;
        playerChunk = PositionToChunkPosition(playerPos);
        playerChunkHolderThread = playerChunk;

        InitComputeShader();
        GetSurroundingChunkPositionsToSendToGPU();
        ComputeSurroundingsGPU();
        ParseVoxelData(voxelDataBufferCPU);
       

        calculationThread = new Thread(calculationThreadLoop);
        calculationThread.Start();
    }

    void Update()
    {
        time = Time.deltaTime;
        playerPos = Vector3Int.CeilToInt(player.transform.position); // for the threads to reference
        playerCameraPos = playerCamera.transform.position;
        cameraRight = playerCamera.transform.right;
        cameraUp = playerCamera.transform.up;
        cameraForward = playerCamera.transform.forward;
        playerChunk = PositionToChunkPosition(playerPos);

        if (generatingChunksDone)
        {
            foreach (ChunkWithMesh cwm in finishedChunks)
            {
                chunkList.Add(cwm);
            }
            finishedChunks.Clear();

            if (chunksReadyForMeshSet.Count > 0)
            {
                SetChunkMeshes();
            }
            if (computeSurroundings)
            {
                ComputeSurroundingsGPU();
                computeSurroundings = false;
                surroundingsJustComputed = true;
            }

            for (int i = 0; i < chunksToRemove.Count; i++)
            {
                availablePositionsInVoxelBufferFromRemovals.Add(chunksToRemove[i].voxelIndex);
                surroundingChunksDataDictionary.Remove(chunksToRemove[i].position);
                chunksHashSet.Remove(chunksToRemove[i].position);
                chunkList.Remove(chunksToRemove[i]);
                voxelBufferChunkCount--;
            }
            chunksToRemove.Clear();

            generatingChunksDone = false; // back to you, other thread
        }


        foreach (ChunkWithMesh c in chunkList)
        {
            Graphics.DrawMesh(c.mesh, c.position, Quaternion.identity, material, 0);
        }
        
    }

    void InitComputeShader()
    {
        computeNoiseShader.SetInt("chunkSize", chunkSize);
        computeNoiseShader.SetInt("chunkRowSize", chunkRowSize);
        computeNoiseShader.SetInt("chunkPlaneSize", chunkRowSize * chunkRowSize);
        computeNoiseShader.SetFloat("threshold", 0.0f);
    }

    void CreateChunkOffsets()
    {
        renderChunkOffsets = new Vector3Int[renderedChunkOffsetsSize];
        int chunkOffsetsCounter = 0;
        Vector3Int startingPos = new Vector3Int(-chunkRenderDistance * chunkRowSize, -chunkRenderDistance * chunkRowSize, -chunkRenderDistance * chunkRowSize); // the bottom, back, left point of the area of octants we check
        int surroundRenderRowSize = chunkRenderDistance * 2;

        for (int x = 0; x < surroundRenderRowSize; x++) // x
        {
            for (int z = 0; z < surroundRenderRowSize; z++) // z
            {
                for (int y = 0; y < surroundRenderRowSize; y++) // y
                {
                    renderChunkOffsets[chunkOffsetsCounter] = startingPos + new Vector3Int(x * chunkRowSize, y * chunkRowSize, z * chunkRowSize);
                    chunkOffsetsCounter++;
                }
            }
        }
    }
    void CreateChunkOffsetsForSurroundingVoxels()
    {
        voxelDataChunkOffsets = new Vector3Int[surroundingVoxelDataSize];
        int offsetsCounter = 0;
        Vector3Int startingPos = new Vector3Int(-surroundingVoxelDataOctantDimensions.x * chunkRowSize, -surroundingVoxelDataOctantDimensions.y * chunkRowSize, -surroundingVoxelDataOctantDimensions.z * chunkRowSize);

        for (int x = 0; x < surroundingVoxelDataOctantDimensions.x * 2; x++) // x
        {
            for (int z = 0; z < surroundingVoxelDataOctantDimensions.z * 2; z++) // z
            {
                for (int y = 0; y < surroundingVoxelDataOctantDimensions.y * 2; y++) // y
                {
                    voxelDataChunkOffsets[offsetsCounter++] = startingPos + new Vector3Int(x * chunkRowSize, y * chunkRowSize, z * chunkRowSize);
                }
            }
        }
    }


    void GetSurroundingChunkPositionsToSendToGPU()
    {
        newChunksForSurroundingChunks.Clear();
        newChunksForSurroundingChunksCount = 0;
        Vector3Int playerChunkPosition = PositionToChunkPosition(playerPos);
        Vector3Int newChunkPosition;
        for (int i = 0; i < surroundingVoxelDataSize; i++)
        {
            newChunkPosition = playerChunkPosition + voxelDataChunkOffsets[i];
            if (!surroundingChunksDataDictionary.ContainsKey(newChunkPosition)) // if we haven't already generated the data for this chunk
            {
                newChunksForSurroundingChunks.Add(newChunkPosition);
                newChunksForSurroundingChunksCount++;
            }
        }
    }

    void ComputeSurroundingsGPU()
    {
        if (newChunksForSurroundingChunksCount > 0)
        {
            chunkPositionsBuffer = new ComputeBuffer(newChunksForSurroundingChunksCount, 12);
            chunkPositionsBuffer.SetData(newChunksForSurroundingChunks.ToArray());
            computeNoiseShader.SetBuffer(0, "ChunkPositionsToGenerate", chunkPositionsBuffer);

            voxelDataBufferCPU = new int[chunkSizeDiv4 * newChunksForSurroundingChunksCount];
            voxelDataReturnBuffer = new ComputeBuffer(chunkSizeDiv4 * newChunksForSurroundingChunksCount, 4);
            voxelDataReturnBuffer.SetData(voxelDataBufferCPU);
            computeNoiseShader.SetBuffer(0, "ChunksVoxelData", voxelDataReturnBuffer);

            float fThreadGroupsX = newChunksForSurroundingChunksCount / (float)gridXSize;
            int threadGroupsX = (int)Mathf.Ceil(fThreadGroupsX);
            if (threadGroupsX < 1)
            {
                threadGroupsX = 1;
            }

            computeNoiseShader.Dispatch(0, threadGroupsX, 1, 1);
            voxelDataReturnBuffer.GetData(voxelDataBufferCPU);
            voxelDataReturnBuffer.Dispose();
            chunkPositionsBuffer.Dispose();
        }
    }
    void ParseVoxelData(int[] voxs)
    {
        if (voxelBufferChunkCount < maxChunks) // if there is room in the buffer for a new chunk
        {
            for (int i = 0; i < newChunksForSurroundingChunksCount; i++)
            {
                // decide where in the voxel buffer to starting adding to
                int voxelStart;
                if (availablePositionsInVoxelBufferFromRemovals.Count > 0)
                {
                    voxelStart = availablePositionsInVoxelBufferFromRemovals[0];
                    availablePositionsInVoxelBufferFromRemovals.RemoveAt(0);
                }
                else
                {
                    voxelStart = voxelBufferChunkCount * chunkSize;
                }

                int counter = voxelStart;
                int mask = 255;

                if (i * chunkSizeDiv4 >= voxs.Length)
                {
                    Debug.Log((i * chunkSizeDiv4) + "acual length of vox: " + voxs.Length + " i / limit: " + i + ", " + newChunksForSurroundingChunksCount);
                }

                if ((voxs[i * chunkSizeDiv4] & 255) == 255) // the flag that stands for emptiness
                {
                    voxelBuffer[voxelStart] = (char)255;
                    for (int j = voxelStart + 1; j < voxelStart + chunkSize; j++)
                    {
                        voxelBuffer[j] = (char)0;
                    }
                }
                else
                {
                    for (int j = 0; j < chunkSizeDiv4; j++) // 4 voxels are stored in each integer
                    {
                        int coords = (i * chunkSizeDiv4) + j;

                        int voxValue = voxs[coords] & mask;
                        voxelBuffer[counter] = (char)voxValue;
                        counter++;

                        voxValue = (voxs[coords] >> 8) & mask;
                        voxelBuffer[counter] = (char)voxValue;
                        counter++;

                        voxValue = (voxs[coords] >> 16) & mask;
                        voxelBuffer[counter] = (char)voxValue;
                        counter++;

                        voxValue = (voxs[coords] >> 24) & mask;
                        voxelBuffer[counter] = (char)voxValue;
                        counter++;
                    }
                }

                surroundingChunksDataDictionary.Add(newChunksForSurroundingChunks[i], voxelStart);
                voxelBufferChunkCount++;
                if (voxelBufferChunkCount >= maxChunks) // if there isn't room in the buffer for a new chunk
                {
                    break;
                }
            }
            newChunksForSurroundingChunksCount = 0;


            /*
            foreach (Vector3Int v in newChunks) // generate some structures!
            {
                if (rnd.Next(1, 100) < 0) // a random chunk
                {
                    int startingVoxel = rnd.Next(0, chunkSize); // a random voxel
                    Vector3Int startingVoxelPos = VoxelIndexTo3D(v, startingVoxel);
                    for (int i = 0; i < testCastle.voxelPositions.Length; i++)
                    {
                        Vector3Int c = new Vector3Int();
                        int vox = 0;
                        VectorToChunkAndVoxelCoords(startingVoxelPos + testCastle.voxelPositions[i], ref c, ref vox);

                        if (surroundingVoxelDataDictionary.ContainsKey(c))
                        {
                            ChunkVoxelData cvd = new ChunkVoxelData();
                            surroundingVoxelDataDictionary[c].voxels[vox] = 1;
                            cvd.voxels = surroundingVoxelDataDictionary[c].voxels;
                            cvd.empty = false;
                            surroundingVoxelDataDictionary[c] = cvd;
                        }
                    }
                }
            }*/
        }

    }
    
    void CreateChunksToRenderInFrustum()
    {
        chunksToRender.Clear();
        Vector3Int playerChunkPosition = PositionToChunkPosition(playerPos);
        calculateFrustum();
        for (int i = 0; i < renderedChunkOffsetsSize; i++)
        {
            if (chunkIsInFrustum(playerChunkPosition + renderChunkOffsets[i]))
            {
                if (!chunksHashSet.Contains(playerChunkPosition + renderChunkOffsets[i])) // if it is not already being rendered
                {
                    chunksToRender.Add(playerChunkPosition + renderChunkOffsets[i]); // to be generated later
                }
            }
        }
    }
   
    void SortChunksToRender(int batchSize)
    {
        closestChunksToRender.Clear();
        for (int i = 0; i < batchSize; i++)
        {
            if (chunksToRender.Count == 0)
            {
                break;
            }
            float shortestDistance = float.PositiveInfinity;
            Vector3Int closestChunkPos = playerPos;
            bool closestChunkAssigned = false;
            foreach (Vector3Int value in chunksToRender)
            {
                uint distance = fastIntDistSqr(playerPos, value);

                if (distance <= shortestDistance && distance <= (unloadDistance * unloadDistance))
                {
                    shortestDistance = distance;
                    closestChunkPos = value;
                    closestChunkAssigned = true;
                }
            }
            if (closestChunkAssigned)
            {
                chunksToRender.Remove(closestChunkPos);
                closestChunksToRender.Add(closestChunkPos);
            }
        }
    }

    void CalculateChunkMeshes()
    {
        if (closestChunksToRender.Count > 0)
        {
            foreach (Vector3Int newKey in closestChunksToRender)
            {
                if (surroundingChunksDataDictionary.ContainsKey(newKey)) // because sometimes the gpu data is not ready for us yet
                {
                    ChunkWithMesh newChunk = new ChunkWithMesh
                    {
                        voxelIndex = surroundingChunksDataDictionary[newKey],
                        position = newKey,
                        empty = voxelBuffer[surroundingChunksDataDictionary[newKey]] == (char)255,
                    };
                    chunksHashSet.Add(newKey);
                    CalculateChunkMesh(ref newChunk);
                    chunksReadyForMeshSet.Add(newChunk);
                }
                
            }
            
            closestChunksToRender.Clear();
        }
    }
    public void CalculateChunkMesh(ref ChunkWithMesh chunk)
    {
        Vector3Int offset = Vector3Int.zero;

        int voxelStart = chunk.voxelIndex;

        int vertexCounter = 0;

        if (!chunk.empty)
        {
            for (int i = voxelStart; i < chunkSize + voxelStart; i++) // for each voxel
            {
                if (voxelBuffer[i] != (char)0)
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
                        leftFaceState = 1; // just setting it to true for now
                    }
                    else
                    {
                        if (voxelBuffer[i - 1] == 0)
                        {
                            leftFaceState = 1;
                        }
                    }
                    // right
                    if (i % chunkRowSize == chunkRowSize - 1)
                    {
                        rightFaceState = 1;
                    }
                    else
                    {
                        if (voxelBuffer[i + 1] == 0)
                        {
                            rightFaceState = 1;
                        }
                    }
                    // back
                    if ((i % chunkPlaneSize) - chunkRowSize < 0)
                    {
                        backFaceState = 1;
                    }
                    else
                    {
                        if (voxelBuffer[i - chunkRowSize] == 0)
                        {
                            backFaceState = 1;
                        }
                    }
                    // front
                    if ((i % chunkPlaneSize) + chunkRowSize >= chunkPlaneSize)
                    {
                        frontFaceState = 1;
                    }
                    else
                    {
                        if (voxelBuffer[i + chunkRowSize] == 0)
                        {
                            frontFaceState = 1;
                        }
                    }
                    // bottom
                    if (i - chunkPlaneSize < voxelStart)
                    {
                        bottomFaceState = 1;
                    }
                    else
                    {
                        if (voxelBuffer[i - chunkPlaneSize] == 0)
                        {
                            bottomFaceState = 1;
                        }
                    }
                    // top
                    if (i + chunkPlaneSize >= chunkSize + voxelStart)
                    {
                        topFaceState = 1;
                    }
                    else
                    {
                        if (voxelBuffer[i + chunkPlaneSize] == 0)
                        {
                            topFaceState = 1;
                        }
                    }



                    if (leftFaceState != 0)
                    {
                        for (int f = 0; f < 6; f++)
                        {
                            chunk.indices.Add(triangle4VertFacePattern[f] + vertexCounter);
                        }
                        for (int f = 0; f < 4; f++)
                        {
                            chunk.vertices.Add(new Vertex(leftFace4Verts[f] + offset, new Color32(0, 255, 255, 255), textureOffsets[f]));
                            vertexCounter++;
                        }
                    }
                    if (rightFaceState != 0)
                    {
                        for (int f = 0; f < 6; f++)
                        {
                            chunk.indices.Add(triangle4VertFacePattern[f] + vertexCounter);
                        }
                        for (int f = 0; f < 4; f++)
                        {
                            chunk.vertices.Add(new Vertex(rightFace4Verts[f] + offset, new Color32(255, 0, 255, 255), textureOffsets[f]));
                            vertexCounter++;
                        }
                    }
                    if (backFaceState != 0)
                    {
                        for (int f = 0; f < 6; f++)
                        {
                            chunk.indices.Add(triangle4VertFacePattern[f] + vertexCounter);
                        }
                        for (int f = 0; f < 4; f++)
                        {
                            chunk.vertices.Add(new Vertex(backFace4Verts[f] + offset, new Color32(255, 255, 0, 255), textureOffsets[f]));
                            vertexCounter++;
                        }
                    }
                    if (frontFaceState != 0)
                    {
                        for (int f = 0; f < 6; f++)
                        {
                            chunk.indices.Add(triangle4VertFacePattern[f] + vertexCounter);
                        }
                        for (int f = 0; f < 4; f++)
                        {
                            chunk.vertices.Add(new Vertex(frontFace4Verts[f] + offset, new Color32(0, 0, 255, 255), textureOffsets[f]));
                            vertexCounter++;
                        }
                    }
                    if (bottomFaceState != 0)
                    {
                        for (int f = 0; f < 6; f++)
                        {
                            chunk.indices.Add(triangle4VertFacePattern[f] + vertexCounter);
                        }
                        for (int f = 0; f < 4; f++)
                        {
                            chunk.vertices.Add(new Vertex(bottomFace4Verts[f] + offset, new Color32(255, 0, 0, 255), textureOffsets[f]));
                            vertexCounter++;
                        }
                    }
                    if (topFaceState != 0)
                    {
                        for (int f = 0; f < 6; f++)
                        {
                            chunk.indices.Add(triangle4VertFacePattern[f] + vertexCounter);
                        }
                        for (int f = 0; f < 4; f++)
                        {
                            chunk.vertices.Add(new Vertex(topFace4Verts[f] + offset, new Color32(255, 255, 255, 255), textureOffsets[f]));
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

            if (vertexCounter < 1)
            {
                chunk.fullButNoVertices = true;
            }
        }
        
    }

    public void SetChunkMeshes()
    {
        foreach (ChunkWithMesh cwm in chunksReadyForMeshSet)
        {
            cwm.mesh = new Mesh();
            cwm.mesh.Clear();
            cwm.mesh.MarkDynamic();
            Vector3[] verts = new Vector3[cwm.vertices.Count];
            Color32[] colors = new Color32[cwm.vertices.Count];
            Vector2[] textCoords = new Vector2[cwm.vertices.Count];
            for (int i = 0; i < cwm.vertices.Count; i++)
            {
                verts[i] = cwm.vertices[i].pos;
                colors[i] = cwm.vertices[i].tint;
                textCoords[i] = cwm.vertices[i].textureCoords;
            }
            cwm.mesh.SetVertices(verts);
            cwm.mesh.SetColors(colors);
            cwm.mesh.SetUVs(0, textCoords);
            cwm.mesh.SetIndices(cwm.indices, MeshTopology.Triangles, 0);

            finishedChunks.Add(cwm);
            cwm.indices.Clear();
            cwm.vertices.Clear();
        }
        chunksReadyForMeshSet.Clear();
    }

    void RemoveChunksTooFarAway(float maxDistance)
    {
        foreach (ChunkWithMesh c in chunkList)
        {
            if (fastIntDistSqr(c.position, playerPos) >= maxDistance * maxDistance)
            {
                chunksToRemove.Add(c);
                //surroundingChunksDataDictionary.Remove(c.position);
                //chunksHashSet.Remove(c.position);
                //voxelBufferChunkCount--;
                /*
                for (int i = 0; i < 6; i++) // for each possible orthogonal neighbor
                {
                    Vector3Int chunkNeighborCoords = c.position + chunkNeighborOperations[i];
                    if (renderedChunksHashSet.Contains(chunkNeighborCoords)) // checking if neighbors exist in loadedChunks
                    {
                        //renderedChunksDictionary[chunkNeighborCoords].existingOrthogonalNeighbors[oppositeSideCoordinates[i]] = false;
                    }
                }*/
            }
        }

        foreach (Vector3Int v in surroundingChunksDataDictionary.Keys)
        {
            if (fastIntDistSqr(v, playerPos) >= dataUnloadDistance * dataUnloadDistance)
            {
                surroundingChunksDictionaryChunksToRemove.Add(v);
            }
        }
        for (int i = 0; i < surroundingChunksDictionaryChunksToRemove.Count; i++)
        {
            availablePositionsInVoxelBufferFromRemovals.Add(surroundingChunksDataDictionary[surroundingChunksDictionaryChunksToRemove[i]]);
            chunksHashSet.Remove(surroundingChunksDictionaryChunksToRemove[i]);
            surroundingChunksDataDictionary.Remove(surroundingChunksDictionaryChunksToRemove[i]);
            voxelBufferChunkCount--;
        }
        surroundingChunksDictionaryChunksToRemove.Clear();
    }


    void calculationThreadLoop()
    {
        Vector3Int lastChunk = playerChunk;
        while (runThread)
        {
            if (!generatingChunksDone) // meaning when we generate new chunks, wait for the main thread to process them and turn this boolean off before continuing
            {

                if (surroundingsJustComputed)
                {
                    ParseVoxelData(voxelDataBufferCPU);
                    surroundingsJustComputed = false;
                }
                CreateChunksToRenderInFrustum();
                SortChunksToRender(batchsize);
                CalculateChunkMeshes();

                playerChunkHolderThread = PositionToChunkPosition(playerPos);
                if (playerChunkHolderThread != lastChunk) // only removing chunks when a player moves out of a chunk
                {
                    lastChunk = playerChunkHolderThread;
                    GetSurroundingChunkPositionsToSendToGPU();
                    RemoveChunksTooFarAway(unloadDistance);
                    computeSurroundings = true;
                }
                


                generatingChunksDone = true;
            }
        }
    }


    Vector3Int PositionToChunkPosition(Vector3Int pos)
    {
        Vector3Int r = new Vector3Int(pos.x - (pos.x % chunkRowSize), pos.y - (pos.y % chunkRowSize), pos.z - (pos.z % chunkRowSize));
        if (pos.x < 0 && pos.x % chunkRowSize != 0)
        {
            r.x -= chunkRowSize;
        }
        if (pos.y < 0 && pos.y % chunkRowSize != 0)
        {
            r.y -= chunkRowSize;
        }
        if (pos.z < 0 && pos.z % chunkRowSize != 0)
        {
            r.z -= chunkRowSize;
        }

        return r;
    }
    Vector3 localPointToWorldSpace(Vector3 right, Vector3 up, Vector3 forward, Vector3 position, Vector3 localPoint)
    {
        Vector3 u1 = localPoint.x * right;
        Vector3 u2 = localPoint.y * up;
        Vector3 u3 = localPoint.z * forward;
        return u1 + u2 + u3 + position;
    }
    float dotProduct(Vector3 a, Vector3 b)
    {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }
    Vector3 crossProduct(Vector3 a, Vector3 b)
    {
        return new Vector3((a.y * b.z) - (b.y * a.z), (b.x * a.z) - (a.x * b.z), (a.x * b.y) - (b.x * a.y));
    }
    bool pointIsOnPositiveSideOfPlane(Vector3 point, Vector3 a, Vector3 b, Vector3 c)
    {
        if (dotProduct(crossProduct(a - b, a - c), point - a) > 0)
        {
            return true;
        }
        return false;
    }
    uint fastIntDistSqr(Vector3Int a, Vector3Int b)
    {
        int x = a.x - b.x;
        int y = a.y - b.y;
        int z = a.z - b.z;
        return (uint)((x * x) + (y * y) + (z * z));
    }
    void calculateFrustum()
    {

        frustumTristanPlanes[0].a = playerCameraPos;
        frustumTristanPlanes[0].b = localPointToWorldSpace(cameraRight, cameraUp, cameraForward, playerCameraPos, new Vector3(3, 3, 10));
        frustumTristanPlanes[0].c = localPointToWorldSpace(cameraRight, cameraUp, cameraForward, playerCameraPos, new Vector3(3, -3, 10));

        frustumTristanPlanes[1].a = playerCameraPos;
        frustumTristanPlanes[1].b = localPointToWorldSpace(cameraRight, cameraUp, cameraForward, playerCameraPos, new Vector3(-3, -3, 10));
        frustumTristanPlanes[1].c = localPointToWorldSpace(cameraRight, cameraUp, cameraForward, playerCameraPos, new Vector3(-3, 3, 10));

        frustumTristanPlanes[2].a = playerCameraPos;
        frustumTristanPlanes[2].b = localPointToWorldSpace(cameraRight, cameraUp, cameraForward, playerCameraPos, new Vector3(-3, 3, 10));
        frustumTristanPlanes[2].c = localPointToWorldSpace(cameraRight, cameraUp, cameraForward, playerCameraPos, new Vector3(3, 3, 10));

        frustumTristanPlanes[3].a = playerCameraPos;
        frustumTristanPlanes[3].b = localPointToWorldSpace(cameraRight, cameraUp, cameraForward, playerCameraPos, new Vector3(3, -3, 10));
        frustumTristanPlanes[3].c = localPointToWorldSpace(cameraRight, cameraUp, cameraForward, playerCameraPos, new Vector3(-3, -3, 10));
    }
    bool chunkIsInFrustum(Vector3Int chunk)
    {
        for (int i = 0; i < 4; i++)
        {
            bool chunkInFrustum = false;
            for (int j = 0; j < 8; j++)
            {
                if (!pointIsOnPositiveSideOfPlane(chunk + cornersOffsets[j], frustumTristanPlanes[i].a, frustumTristanPlanes[i].b, frustumTristanPlanes[i].c))
                {
                    chunkInFrustum = true;
                    break;
                }
            }
            if (!chunkInFrustum)
            {
                return false;
            }
        }
        return true;
    }


    private void OnApplicationQuit()
    {
        runThread = false;
        chunkPositionsBuffer.Dispose();
        voxelDataReturnBuffer.Dispose();
    }

}
