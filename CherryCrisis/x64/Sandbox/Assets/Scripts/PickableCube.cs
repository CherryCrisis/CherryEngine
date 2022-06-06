using CCEngine;

namespace CCScripting
{
    public class PickableCube : Behaviour
    {
        public PickableCube(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

        Transform transform;

        public void Start()
        {
            transform = GetBehaviour<Transform>();
        }

        public void Pick(Transform parent) 
        {
            transform.SetParent(parent);
            transform.SetPosition(Vector3.Zero);
            transform.SetRotation(Quaternion.Identity);
        }

        public void Throw() 
        {
            transform.SetParent(null, true, true);  
            // add force
        }
    }
}