using CCEngine;

namespace CCScripting
{
    public class Wardrobe : Behaviour
    {
        public Wardrobe(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) { }

        Transform leftDoor  = null;
        Transform rightDoor = null;

        AudioEmitter emitter;
        public bool opened = false;
        public bool isMoving = false;

        Timer respawnTimer = new Timer(1f);
        Timer knockTimer = new Timer(8f);
        public float respawnTime = 1f;

        public float movementScale = 5f;
        public void Start() 
        {
            respawnTimer.totalTime = respawnTime;
            emitter.AddSound("Assets/Sounds/wardrobe_locked.wav");
            emitter.SetLooping(false);
            emitter.SetSpatialized(true);
        }

        public void SetInMovement()
        {
            isMoving = true;
            emitter.Stop();
        }

        public void Update() 
        {
            float dt = Time.GetDeltaTime();
            if (isMoving && !opened) 
            {
                float offset = movementScale * dt;
                
                respawnTimer.Tick(dt);

                Vector3 leftDoorRot = leftDoor.GetEuler(); 
                leftDoor.eulerAngles = new Vector3( leftDoorRot.x, leftDoorRot.y + offset, leftDoorRot.z);

                Vector3 rightDoorRot = rightDoor.GetEuler();
                rightDoor.eulerAngles = new Vector3(rightDoorRot.x, rightDoorRot.y - offset, rightDoorRot.z);

                if (respawnTimer.CheckAndReset()) 
                {
                    opened = true;
                    isMoving = false;
                }
            }
            
            if (knockTimer.Tick(dt))
                emitter.Play();
        }
    }
}