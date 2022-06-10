using CCEngine;

namespace CCScripting
{
    public class PickableCube : Behaviour
    {
        public PickableCube(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) { }

        Transform transform;
        Rigidbody rb;
        BoxCollider collider;
        AudioEmitter emitter;
        
        bool threw = false;
        bool collided = false;

        Cell cell;
        Vector3 spawnLocation = Vector3.Zero;
        Quaternion spawnRot; 

        float elapsedTime = 0f;
        float respawnTime = 1f;

        public void Start()
        {
            transform = GetBehaviour<Transform>(); 
            rb = GetBehaviour<Rigidbody>();
            collider = GetBehaviour<BoxCollider>();
            emitter = GetBehaviour<AudioEmitter>();
            SetRespawnPosition();
        }

        public void Pick(Transform parent)
        {
            ResetRespawnVariables();

            collider.SetTrigger(true);
            rb.SetKinematic(true);
            transform.SetParent(parent);
            transform.SetPosition(Vector3.Zero);
            transform.SetRotation(Quaternion.Identity);
        }

        public void Throw(Vector3 direction, float strength)
        {
            threw = true;
            transform.SetParent(null, true, true);
            collider.SetTrigger(false);
            rb.SetKinematic(false);

            rb.AddForce(direction * strength, EForceMode.eFORCE);
        }

        public void Update() 
        {
            if (!collided)
                return;

            if (elapsedTime < respawnTime)
            {
                elapsedTime += Time.GetDeltaTime();
            }
            else
            {
                ResetRespawnVariables();
                Respawn();
            }
        }
        public void OnCollisionEnter(Entity other) 
        {
            if (!threw)
                return;

            collided = true;
            emitter?.Play();
        }

        public void Respawn() 
        {
            rb.SetVelocity(Vector3.Zero);
            if (host.m_cell != cell)
                host.SetCell(cell);

            transform.position = spawnLocation;
            transform.rotation = spawnRot;
        }

        public void SetRespawnPosition() 
        {
            Debug.Log(ELogType.INFO, "Settings respawn position");
            spawnLocation = transform.position;
            spawnRot      = transform.rotation;
            cell          = host.m_cell;
        }

        void ResetRespawnVariables() 
        {
            threw = false;
            collided = false;
            elapsedTime = 0f;
        }
    }
}