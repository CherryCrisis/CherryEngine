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
        PortalTeleporterComponent traveler;

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

            traveler = GetBehaviour<PortalTeleporterComponent>();

            SetRespawnPosition();
        }

        public void Pick(Transform parent)
        {
            traveler.SetActive(false);

            ResetRespawnVariables();

            collider.SetTrigger(true);
            rb.SetKinematic(true);
            transform.SetParent(parent);
            transform.SetPosition(Vector3.Zero);
            transform.SetRotation(Quaternion.Identity);

            parent.GetParent()?.GetParent()?.GetBehaviour<PortalTeleporterComponent>()?.ReloadEntitiesClone();

            Debug.Info("Parent = " + parent.GetParent() + " || Le Parent = " + parent.GetParent().GetParent());
        }

        public void Throw(Vector3 direction, float strength)
        {
            traveler.SetActive(true);

            Transform parent = transform.GetParent();
            threw = true;
            transform.SetParent(null, true, true);
            collider.SetTrigger(false);
            rb.SetKinematic(false);

            rb.AddForce(direction * strength, EForceMode.eFORCE);

            parent.GetParent()?.GetParent()?.GetBehaviour<PortalTeleporterComponent>()?.ReloadEntitiesClone();
            Debug.Info("Parent = " + parent.GetParent() + " || Le Parent = " + parent.GetParent().GetParent());


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