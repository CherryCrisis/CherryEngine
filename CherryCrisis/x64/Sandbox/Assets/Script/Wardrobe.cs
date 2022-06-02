using CCEngine;

namespace CCScripting
{
    public class Wardrobe : Behaviour
    {
        public Wardrobe(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) { }

        Transform leftDoor;
        Transform rightDoor;

        bool opened = false;

        public void ToggleState()
        {
            // play open sound
            opened = !opened;

            if (opened)
            {
                Debug.GetInstance().Log(ELogType.INFO, "Door opened");
            }
            else
            {
                Debug.GetInstance().Log(ELogType.INFO, "Door closed");
            }
        }

        public void Update() 
        {
            // need homemade timeline
        }
    }
}