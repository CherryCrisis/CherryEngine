using CCEngine;

namespace CCScripting
{
    public class PickableCube : Behaviour
    {
        public PickableCube(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

        Transform transform;
        Rigidbody rb;
        BoxCollider collider;

        public void Start()
        {
            transform = GetBehaviour<Transform>();
            rb = GetBehaviour<Rigidbody>();
            collider = GetBehaviour<BoxCollider>();
        }

        public void Pick(Transform parent) 
        {
            collider.SetTrigger(true);
            rb.SetKinematic(true);  
            transform.SetParent(parent);
            transform.SetPosition(Vector3.Zero);
            transform.SetRotation(Quaternion.Identity); 
        }

        public void Throw(Vector3 direction, float strength) 
        {
            transform.SetParent(null, true, true);
            collider.SetTrigger(false);
            rb.SetKinematic(false);
            
            rb.AddForce(direction * strength, EForceMode.eFORCE); 
        }
    }
}