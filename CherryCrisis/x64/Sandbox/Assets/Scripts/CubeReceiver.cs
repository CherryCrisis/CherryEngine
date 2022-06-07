using CCEngine;

namespace CCScripting
{
    public class CubeReceiver : Behaviour
    {
        public CubeReceiver(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

        PortalSwitcher switcher;

        public void OnTriggerEnter(Entity other) 
        {
            // may replace this by tags
            PickableCube cube = other.GetBehaviour<PickableCube>();
            if (cube != null) 
            {
                if (switcher == null)
                    Debug.Log(ELogType.ERROR, "There is no PortalSwitch to interact with");
                else
                    switcher.Switch();
            }
        }
    }
}