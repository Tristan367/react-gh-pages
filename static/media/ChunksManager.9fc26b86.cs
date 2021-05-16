using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;
using System.Linq;
using UnityEditor;

public class ChunksManager : MonoBehaviour
{
    [System.NonSerialized] public Dictionary<Vector3Int, Chunk> renderedChunksDictionary = new Dictionary<Vector3Int, Chunk>();
    HashSet<Vector3Int> chunksToRenderHash = new HashSet<Vector3Int>();
    List<Vector3Int> chunksToRecalculate = new List<Vector3Int>();
    //List<Vector3Int> closestChunksToGenerate = new List<Vector3Int>();
    HashSet<Vector3Int> closestChunksToRender = new HashSet<Vector3Int>();
    [System.NonSerialized] public Dictionary<Vector3Int, Chunk> finishedChunks = new Dictionary<Vector3Int, Chunk>();
    List<Vector3Int> chunksToRemove = new List<Vector3Int>();
    List<Vector3Int> surroundingVoxelDataDictionaryChunksToRemove = new List<Vector3Int>();


    public struct ChunkVoxelData
    {
        public char[] voxels;
        public bool empty;
    };

    [System.NonSerialized] public Dictionary<Vector3Int, ChunkVoxelData> surroundingVoxelDataDictionary = new Dictionary<Vector3Int, ChunkVoxelData>();

    List<Vector3Int> voxelDataToFrustumChunks = new List<Vector3Int>();
    List<Vector3Int> newChunks = new List<Vector3Int>();
    int newChunksCount = 0;

    Vector3Int[] chunkNeighborOperations;
    int[] oppositeSideCoordinates =
    {
        1,
        0,
        3,
        2,
        5,
        4
    };

    public GameObject player;
    public GameObject playerCamera;
    Camera actualCameraComponent;

    Plane[] frustumPlanes;
    Vector3Int[] renderChunkOffsets;
    int renderedChunkOffsetsSize;
    const int chunkRenderDistance = 16;
    List<Vector3Int> chunksInFrustum = new List<Vector3Int>();
    float unloadDistance;
    const int batchsize = 25;

    const int chunkRowSize = 16;
    const int chunkPlaneSize = chunkRowSize * chunkRowSize;
    const int chunkSize = chunkRowSize * chunkPlaneSize;
    const int chunkSizeDiv4 = chunkSize / 4;


    Vector3Int[] voxelDataChunkOffsets;
    Vector3Int largestStructureOctantDimensions = new Vector3Int(2,2,2); // in chunks
    int largestStructureLargestDimension = 2;
    int surroundingVoxelDataSize;
    Vector3Int surroundingVoxelDataOctantDimensions;
    float dataUnloadDistance;

    Vector3Int playerPos; // for multi threading, the chunk coordinate the player is in
    Vector3 playerCameraPos;
    Vector3 playerCameraForward;
    Vector3Int playerChunk;

    public Material material;

    [System.NonSerialized] public Vector3Int[] leftFace4Verts = new Vector3Int[]
        {
            new Vector3Int(0,0,1),
            new Vector3Int(0,1,1),
            new Vector3Int(0,1,0),
            new Vector3Int(0,0,0),
        };
    [System.NonSerialized] public Vector3Int[] rightFace4Verts = new Vector3Int[]
        {
            new Vector3Int(1,0,0),
            new Vector3Int(1,1,0),
            new Vector3Int(1,1,1),
            new Vector3Int(1,0,1),
        };
    [System.NonSerialized] public Vector3Int[] backFace4Verts = new Vector3Int[]
        {
            new Vector3Int(0,0,0),
            new Vector3Int(0,1,0),
            new Vector3Int(1,1,0),
            new Vector3Int(1,0,0),
        };
    [System.NonSerialized] public Vector3Int[] frontFace4Verts = new Vector3Int[]
        {
            new Vector3Int(1,0,1),
            new Vector3Int(1,1,1),
            new Vector3Int(0,1,1),
            new Vector3Int(0,0,1),
        };
    [System.NonSerialized] public Vector3Int[] bottomFace4Verts = new Vector3Int[]
        {
            new Vector3Int(1,0,0),
            new Vector3Int(1,0,1),
            new Vector3Int(0,0,1),
            new Vector3Int(0,0,0),
        };
    [System.NonSerialized] public Vector3Int[] topFace4Verts = new Vector3Int[]
        {
            new Vector3Int(0,1,0),
            new Vector3Int(0,1,1),
            new Vector3Int(1,1,1),
            new Vector3Int(1,1,0),
        };
    [System.NonSerialized] public int[] triangle4VertFacePattern = new int[]
        {
            0,1,2, 0,2,3
        };

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

    Thread calculationThread;
    bool generatingChunksDone = false;
    bool runThread = true;
    float time;

    VoxelPointImport testCastle;
    System.Random rnd;

    public ComputeShader computeNoiseShader;
    ComputeBuffer chunkPositionsBuffer;
    Vector3Int[] chunkPositionsToSendToGPU;
    ComputeBuffer voxelDataReturnBuffer;
    int[] voxelDataBufferCPU;
    const int gridXSize = 256;

    struct TristanPlane
    {
        public Vector3 a, b, c, n;
    }
    TristanPlane[] frustumTristanPlanes = new TristanPlane[4];

    Vector3 cameraRight, cameraUp, cameraForward;

    private void Start()
    {
        rnd = new System.Random();
        playerPos = Vector3Int.CeilToInt(player.transform.position); // for the threads to reference
        playerCameraPos = playerCamera.transform.position;
        playerCameraForward = playerCamera.transform.forward;
        cameraRight = playerCamera.transform.right;
        cameraUp = playerCamera.transform.up;
        actualCameraComponent = playerCamera.GetComponent<Camera>();
        playerChunk = PositionToChunkPosition(playerPos);

        


        unloadDistance = chunkRenderDistance * chunkRowSize;
        dataUnloadDistance = (chunkRenderDistance + largestStructureLargestDimension) * chunkRowSize * 1.733f;

        int renderedChunkOffsetsRowSize = chunkRenderDistance * 2;
        renderedChunkOffsetsSize = (int)Mathf.Pow(renderedChunkOffsetsRowSize, 3);
        CreateChunkOffsets();
        surroundingVoxelDataOctantDimensions = new Vector3Int(chunkRenderDistance + largestStructureOctantDimensions.x, chunkRenderDistance + largestStructureOctantDimensions.y, chunkRenderDistance + largestStructureOctantDimensions.z);
        surroundingVoxelDataSize = surroundingVoxelDataOctantDimensions.x * surroundingVoxelDataOctantDimensions.y * surroundingVoxelDataOctantDimensions.z * 8;
        CreateChunkOffsetsForSurroundingVoxels();
        chunkNeighborOperations = new Vector3Int[]
        {
            new Vector3Int(-chunkRowSize, 0, 0),
            new Vector3Int(chunkRowSize, 0, 0),
            new Vector3Int(0, 0, -chunkRowSize),
            new Vector3Int(0, 0, chunkRowSize),
            new Vector3Int(0, -chunkRowSize, 0),
            new Vector3Int(0, chunkRowSize, 0),

        };

        testCastle = new VoxelPointImport();
        testCastle.UseRawVoxelData(Application.dataPath + "/castle.ply");

        calculateFrustum();
        time = Time.deltaTime;


        InitComputeShader();
        GetSurroundingChunkPositionsToSendToGPU();
        ComputeSurroundingsGPU();
        AssignGPUdataTosurroundingData(voxelDataBufferCPU);
        newChunks.Clear();
        newChunksCount = 0;

        calculationThread = new Thread(calculationThreadLoop);
        calculationThread.Start();

        RenderSettings.fogDensity = (1 / unloadDistance);

    }

    public void Update()
    {
        time = Time.deltaTime;
        playerPos = Vector3Int.FloorToInt(player.transform.position); // for the threads to reference
        playerCameraPos = playerCamera.transform.position;
        playerCameraForward = playerCamera.transform.forward;
        cameraRight = playerCamera.transform.right;
        cameraUp = playerCamera.transform.up;

        if (generatingChunksDone) {
            foreach (KeyValuePair<Vector3Int, Chunk> kv in finishedChunks) {
                renderedChunksDictionary.Add(kv.Key, kv.Value);
            } 
            finishedChunks.Clear();
            
            for (int i = 0; i < chunksToRemove.Count; i++) {
                renderedChunksDictionary.Remove(chunksToRemove[i]);
            }
            chunksToRemove.Clear();

            ComputeSurroundingsGPU();
            generatingChunksDone = false;
        }
        

        foreach (Chunk chunk in renderedChunksDictionary.Values) {
            if (chunk.ready) {
                if (!chunk.empty) {
                    Graphics.DrawMesh(chunk.mesh, chunk.position, Quaternion.identity, material, 0);
                }
            }
            else { // make it ready
                if (!chunk.empty) {
                    chunk.SetMeshData();
                    Graphics.DrawMesh(chunk.mesh, chunk.position, Quaternion.identity, material, 0);
                }
                chunk.ready = true;
            }
        }
    }


    void InitComputeShader()
    {
        computeNoiseShader.SetInt("chunkSize", chunkSize);
        computeNoiseShader.SetInt("chunkRowSize", chunkRowSize);
        computeNoiseShader.SetInt("chunkPlaneSize", chunkRowSize * chunkRowSize);
        computeNoiseShader.SetFloat("threshold", .1f);
    }
    void GetSurroundingChunkPositionsToSendToGPU()
    {
        newChunks.Clear();
        newChunksCount = 0;
        Vector3Int playerChunkPosition = PositionToChunkPosition(playerPos);
        Vector3Int newChunkPosition;
        for (int i = 0; i < surroundingVoxelDataSize; i++)
        {
            newChunkPosition = playerChunkPosition + voxelDataChunkOffsets[i];
            if (!surroundingVoxelDataDictionary.ContainsKey(newChunkPosition)) // if we haven't already generated the data for this chunk
            {
                newChunks.Add(newChunkPosition);
                newChunksCount++;
            } 
        }
    }
    void ComputeSurroundingsGPU()
    {
        if (newChunksCount > 0)
        {
            chunkPositionsBuffer = new ComputeBuffer(newChunksCount, 12);
            chunkPositionsBuffer.SetData(newChunks.ToArray());
            computeNoiseShader.SetBuffer(0, "ChunkPositionsToGenerate", chunkPositionsBuffer);

            voxelDataBufferCPU = new int[chunkSizeDiv4 * newChunksCount];
            voxelDataReturnBuffer = new ComputeBuffer(chunkSizeDiv4 * newChunksCount, 4);
            voxelDataReturnBuffer.SetData(voxelDataBufferCPU);
            computeNoiseShader.SetBuffer(0, "ChunksVoxelData", voxelDataReturnBuffer);

            float fThreadGroupsX = newChunksCount / (float)gridXSize;
            int threadGroupsX = (int)Mathf.Ceil(fThreadGroupsX);
            if (threadGroupsX < 1)
            {
                threadGroupsX = 1;
            }

            computeNoiseShader.Dispatch(0, threadGroupsX, 1, 1);
            voxelDataReturnBuffer.GetData(voxelDataBufferCPU);
            voxelDataReturnBuffer.Dispose();
            chunkPositionsBuffer.Dispose();


            /*
            chunkPositionsBuffer = new ComputeBuffer(newChunks.Count, 12);
            chunkPositionsBuffer.SetData(newChunks.ToArray());
            computeNoiseShader.SetBuffer(0, "ChunkPositionsToGenerate", chunkPositionsBuffer);

            voxelDataBufferCPU = new int[chunkSize * newChunks.Count];
            voxelDataReturnBuffer = new ComputeBuffer(chunkSize * newChunks.Count, 4);
            voxelDataReturnBuffer.SetData(voxelDataBufferCPU);
            computeNoiseShader.SetBuffer(0, "ChunksVoxelData", voxelDataReturnBuffer);

            float fThreadGroupsX = newChunks.Count / 64.0f;
            int threadGroupsX = (int)Mathf.Ceil(fThreadGroupsX);
            if (threadGroupsX < 1)
            {
                threadGroupsX = 1;
            }

            computeNoiseShader.Dispatch(0, threadGroupsX, 1, 1);
            voxelDataReturnBuffer.GetData(voxelDataBufferCPU);
            voxelDataReturnBuffer.Dispose();
            chunkPositionsBuffer.Dispose();*/
        }
        
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


    
    void calculateFrustum()
    {
        frustumTristanPlanes[0].a = playerCameraPos;
        frustumTristanPlanes[0].b = localPointToWorldSpace(cameraRight, cameraUp, playerCameraForward, playerCameraPos, new Vector3(3, 3, 10));
        frustumTristanPlanes[0].c = localPointToWorldSpace(cameraRight, cameraUp, playerCameraForward, playerCameraPos, new Vector3(3, -3, 10));

        frustumTristanPlanes[1].a = playerCameraPos;
        frustumTristanPlanes[1].b = localPointToWorldSpace(cameraRight, cameraUp, playerCameraForward, playerCameraPos, new Vector3(-3, -3, 10));
        frustumTristanPlanes[1].c = localPointToWorldSpace(cameraRight, cameraUp, playerCameraForward, playerCameraPos, new Vector3(-3, 3, 10));

        frustumTristanPlanes[2].a = playerCameraPos;
        frustumTristanPlanes[2].b = localPointToWorldSpace(cameraRight, cameraUp, playerCameraForward, playerCameraPos, new Vector3(-3, 3, 10));
        frustumTristanPlanes[2].c = localPointToWorldSpace(cameraRight, cameraUp, playerCameraForward, playerCameraPos, new Vector3(3, 3, 10));

        frustumTristanPlanes[3].a = playerCameraPos;
        frustumTristanPlanes[3].b = localPointToWorldSpace(cameraRight, cameraUp, playerCameraForward, playerCameraPos, new Vector3(3, -3, 10));
        frustumTristanPlanes[3].c = localPointToWorldSpace(cameraRight, cameraUp, playerCameraForward, playerCameraPos, new Vector3(-3, -3, 10));
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

    void AssignGPUdataTosurroundingData(int[] voxs)
    {
        for (uint i = 0; i < newChunksCount; i++)
        {
            ChunkVoxelData cvdt = new ChunkVoxelData();
            cvdt.voxels = new char[chunkSize]; // the bytes will be parsed into ints for now

            cvdt.empty = false;
            if ((voxs[i * chunkSizeDiv4] & 255) == 255) // the flag that stands for emptiness
            {
                cvdt.empty = true;
                voxs[i * chunkSizeDiv4] &= ~255; // undoing the flag because it is no longer needed
            }

            uint counter = 0;
            int mask = 255;

            for (uint j = 0; j < chunkSizeDiv4; j++) // for each integer
            {
                uint coords = (i * chunkSizeDiv4) + j;
                int voxValue = voxs[coords] & mask;
                cvdt.voxels[counter] = (char)voxValue;
                counter++;

                voxValue = (voxs[coords] >> 8) & mask;
                cvdt.voxels[counter] = (char)voxValue;
                counter++;

                voxValue = (voxs[coords] >> 16) & mask;
                cvdt.voxels[counter] = (char)voxValue;
                counter++;

                voxValue = (voxs[coords] >> 24) & mask;
                cvdt.voxels[counter] = (char)voxValue;
                counter++;
            }

            surroundingVoxelDataDictionary.Add(newChunks[(int)i], cvdt);
        }

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
   
    bool ChunkIsInFrustum(Vector3Int chunk)
    {
        calculateFrustum();
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
    void CreateChunksToRenderInFrustum()
    {
        chunksToRenderHash.Clear();
        Vector3Int playerChunkPosition = PositionToChunkPosition(playerPos);
        for (int i = 0; i < renderedChunkOffsetsSize; i++)
        {
            if (ChunkIsInFrustum(playerChunkPosition + renderChunkOffsets[i]))
            {
                if (!renderedChunksDictionary.ContainsKey(playerChunkPosition + renderChunkOffsets[i]))
                {
                    chunksToRenderHash.Add(playerChunkPosition + renderChunkOffsets[i]); // to be generated later
                }
            }
        }
    }
    uint fastIntDistSqr(Vector3Int a, Vector3Int b)
    {
        int x = a.x - b.x;
        int y = a.y - b.y;
        int z = a.z - b.z;
        return (uint)((x * x) + (y * y) + (z * z));
    }
    void SortChunksToRender(int batchSize)
    {
        closestChunksToRender.Clear();
        for (int i = 0; i < batchSize; i++)
        {
            if (chunksToRenderHash.Count == 0)
            {
                break;
            }
            float shortestDistance = float.PositiveInfinity;
            Vector3Int closestChunkPos = playerPos;
            bool closestChunkAssigned = false;
            foreach (Vector3Int value in chunksToRenderHash)
            {
                float distance = fastIntDistSqr(playerPos, value); // Vector3.Distance(playerPos, value);

                if (distance <= shortestDistance && distance <= (unloadDistance * unloadDistance))
                {
                    shortestDistance = distance;
                    closestChunkPos = value;
                    closestChunkAssigned = true;
                }
            }
            if (closestChunkAssigned)
            {
                chunksToRenderHash.Remove(closestChunkPos);
                closestChunksToRender.Add(closestChunkPos);
            }
        }
    }
    void CalculateChunkMeshes()
    {
        finishedChunks.Clear();
        chunksToRecalculate.Clear();
        foreach (Vector3Int newKey in closestChunksToRender) 
        {
            Chunk newChunk = new Chunk
            {
                chunkRowSize = chunkRowSize,
                chunkPlaneSize = chunkPlaneSize,
                chunkSize = chunkSize,
                chunksManager = this,
                position = newKey,
                voxels = surroundingVoxelDataDictionary[newKey].voxels,
                empty = surroundingVoxelDataDictionary[newKey].empty,
                neighboringChunkVoxels = new char[6][]
            };

            // check if neighbors of this chunk exist, and if so, change both ours and their appropriate chunk neighbor bools
            for (int i = 0; i < 6; i++) // for each possible orthogonal neighbor
            {
                Vector3Int chunkNeighborCoords = newKey + chunkNeighborOperations[i];
                if (renderedChunksDictionary.ContainsKey(chunkNeighborCoords)) // checking if neighbors exist in loadedChunks
                {
                    newChunk.existingOrthogonalNeighbors[i] = true; // setting our bool to true
                    renderedChunksDictionary[chunkNeighborCoords].existingOrthogonalNeighbors[oppositeSideCoordinates[i]] = true; // setting the bool of our neighbor to true

                    newChunk.neighboringChunkVoxels[i] = renderedChunksDictionary[chunkNeighborCoords].voxels;
                    renderedChunksDictionary[chunkNeighborCoords].neighboringChunkVoxels[oppositeSideCoordinates[i]] = newChunk.voxels;

                    chunksToRecalculate.Add(chunkNeighborCoords);
                }
                else if (finishedChunks.ContainsKey(chunkNeighborCoords)) // checking if neighbors exist in generatedChunks
                {
                    newChunk.existingOrthogonalNeighbors[i] = true; // setting our bool to true
                    finishedChunks[chunkNeighborCoords].existingOrthogonalNeighbors[oppositeSideCoordinates[i]] = true; // setting the bool of our neighbor to true

                    newChunk.neighboringChunkVoxels[i] = finishedChunks[chunkNeighborCoords].voxels;
                    finishedChunks[chunkNeighborCoords].neighboringChunkVoxels[oppositeSideCoordinates[i]] = newChunk.voxels;
                }

            }
            finishedChunks.Add(newKey, newChunk);
        }
        closestChunksToRender.Clear();
        foreach (Chunk c in finishedChunks.Values)
        {
            c.InitializeChunk();
            c.CalculateMeshData();
        }
        for (int i = 0; i < chunksToRecalculate.Count; i++)
        {
            renderedChunksDictionary[chunksToRecalculate[i]].RecalculateMeshIfAllNeighborsExist();
        }
    }
    void RemoveChunksTooFarAway(float maxDistance)
    {
        chunksToRemove.Clear();
        foreach (Vector3Int v in renderedChunksDictionary.Keys)
        {
            if (Vector3.Distance(v, playerPos) >= maxDistance)
            {
                chunksToRemove.Add(v);

                for (int i = 0; i < 6; i++) // for each possible orthogonal neighbor
                {
                    Vector3Int chunkNeighborCoords = v + chunkNeighborOperations[i];
                    if (renderedChunksDictionary.ContainsKey(chunkNeighborCoords)) // checking if neighbors exist in loadedChunks
                    {
                        renderedChunksDictionary[chunkNeighborCoords].existingOrthogonalNeighbors[oppositeSideCoordinates[i]] = false;
                    }
                }
            }
        }

        surroundingVoxelDataDictionaryChunksToRemove.Clear();
        foreach (Vector3Int v in surroundingVoxelDataDictionary.Keys)
        {
            if (Vector3.Distance(v, playerPos) >= dataUnloadDistance)
            {
               surroundingVoxelDataDictionaryChunksToRemove.Add(v);
            }
        }
        //Debug.Log("removed " + surroundingVoxelDataDictionaryChunksToRemove.Count + " from surroundingVoxelDataDictionary.");
        for (int i = 0; i < surroundingVoxelDataDictionaryChunksToRemove.Count; i++)
        {
            surroundingVoxelDataDictionary.Remove(surroundingVoxelDataDictionaryChunksToRemove[i]);
        }
    }

    void calculationThreadLoop()
    {
        Vector3Int playerChunkHolder;
        while (runThread)
        {
            if (!generatingChunksDone) // meaning when we generate new chunks, wait for the main thread to process them and turn this boolean off before continuing
            {
                CreateChunksToRenderInFrustum();
                SortChunksToRender(batchsize);

                playerChunkHolder = PositionToChunkPosition(playerPos);
                if (playerChunkHolder != playerChunk) // only removing chunks when a player moves out of a chunk
                {
                    playerChunk = playerChunkHolder;
                    RemoveChunksTooFarAway(unloadDistance);
                }

                AssignGPUdataTosurroundingData(voxelDataBufferCPU);
                CalculateChunkMeshes();

                GetSurroundingChunkPositionsToSendToGPU();

                generatingChunksDone = true;
            }
        }
    }



    private void OnApplicationQuit()
    {
        runThread = false;
        chunkPositionsBuffer.Dispose();
        voxelDataReturnBuffer.Dispose();
    }
}
