using CCEngine;

namespace CCScripting
{
    public class CubeSpawner : Behaviour
    {
        public CubeSpawner(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        PickableCube instanciedCube;

        public void SpawnCube() 
        {
            instanciedCube.Respawn(); 
        }

        public void Start()
        {
            if (instanciedCube == null)
                Debug.Log(ELogType.ERROR, "instanciedCube is not setup !");
        }
    }
}