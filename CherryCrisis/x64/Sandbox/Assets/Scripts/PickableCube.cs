using CCEngine;

namespace CCScripting
{
    public class PickableCube : Behaviour
    {
        public PickableCube(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

        Transform transform;
        Rigidbody rb;

        public void Start()
        {
            transform = GetBehaviour<Transform>();
            rb = GetBehaviour<Rigidbody>();
        }

        public void Pick(Transform parent) 
        {
            rb.SetKinematic(true);
            transform.SetParent(parent);
            transform.SetPosition(Vector3.Zero);
            transform.SetRotation(Quaternion.Identity);
        }

        public void Throw() 
        {
            transform.SetParent(null, true, true);
            rb.SetKinematic(false);
            rb.AddForce(Vector3.Up * 500f, EForceMode.eFORCE);
        }
    }
}