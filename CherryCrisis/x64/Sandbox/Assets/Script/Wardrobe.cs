using CCEngine;

namespace CCScripting
{
    public class Wardrobe : Behaviour
    {
        public Wardrobe(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) { }

        Transform leftDoor;
        Transform rightDoor;

        public bool opened = false;
        public bool isMoving = false;

        public float elapsedTime = 0f;
        public float totalTime   = 1f;
        public float movementScale = 5f;
        public void Start() 
        {

        }

        public void SetInMovement()
        {
            isMoving = true;
        }

        public void Update() 
        {
            if (isMoving && !opened) 
            {
                float dt = Time.GetDeltaTime();
                float offset = movementScale * dt;
                elapsedTime += dt;

                Vector3 leftDoorRot = leftDoor.GetEuler();
                leftDoor.SetRotation(new Vector3( leftDoorRot.x, leftDoorRot.y + offset, leftDoorRot.z));

                Vector3 rightDoorRot = rightDoor.GetEuler();
                rightDoor.SetRotation(new Vector3(rightDoorRot.x, rightDoorRot.y - offset, rightDoorRot.z));

                if (elapsedTime >= totalTime) 
                {
                    opened = true;
                    isMoving = false;
                    elapsedTime = 0f;
                }
            }
        }
    }
}