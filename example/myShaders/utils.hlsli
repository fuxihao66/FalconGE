
namespace RND{
    uint SeedThread(uint seed)
    {
        // Thomas Wang hash 
        // Ref: http://www.burtleburtle.net/bob/hash/integer.html
        seed = (seed ^ 61) ^ (seed >> 16);
        seed *= 9;
        seed = seed ^ (seed >> 4);
        seed *= 0x27d4eb2d;
        seed = seed ^ (seed >> 15);
        return seed;
    }

    // Generate a random 32-bit integer
    uint Random(inout uint state)
    {
        // Xorshift algorithm from George Marsaglia's paper.
        state ^= (state << 13);
        state ^= (state >> 17);
        state ^= (state << 5);
        return state;
    }

    float SampleUniform01(inout uint state){

        return Random(state) / float(0xffffffff);

    }


    uint CreateRandomSeed(uint2 xyDim, uint2 XY, uint Z){
        return Z*xyDim.x*xyDim.y + XY.y * xyDim.x  + XY.x;
    }


    // 使用方法： 创建seed，通过seed调用SampleUniform01
}

