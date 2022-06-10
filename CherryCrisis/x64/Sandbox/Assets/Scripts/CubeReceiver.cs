using CCEngine;
using System;

namespace CCScripting
{
    public class CubeReceiver : Behaviour
    {
        public CubeReceiver(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

        //PortalSwitcher switcher;

        public void OnTriggerEnter(Entity other) 
        {
            // may replace this by tags
            PickableCube cube = other.GetBehaviour<PickableCube>();
            if (cube != null) 
            {
                /*if (switcher == null)
                      Debug.Log(ELogType.ERROR, "There is no PortalSwitch to interact with");
                  else
                      switcher.Switch();*/

                Switch();
            }
        }

        PortalComponent sourcePortal;
        PortalComponent portal_0;
        PortalComponent portal_1;

        AudioEmitter emitter;

        bool flipPortal = false;

        // if active then link portal_1
        bool active = false;

        private Transform transformSourcePortal;
        void Start()
        {
            emitter = AddBehaviour<AudioEmitter>();
            emitter.AddSound("Assets/Sounds/portal_switcher_interact.wav");
            emitter.SetSpatialized(true);
            emitter.SetLooping(false);
            if (sourcePortal != null)
                transformSourcePortal = sourcePortal.GetBehaviour<Transform>();
        }

        void FlipPortal()
        {
            Quaternion quat = Quaternion.FromAxisAngle(transformSourcePortal.Up(), (float)Math.PI);
            transformSourcePortal.SetRotation(transformSourcePortal.GetRotation() * quat);
        }

        public void Switch()
        {
            active = !active;
            if (emitter != null)
                emitter.Play();

            if (portal_0 == null || portal_1 == null || sourcePortal == null)
                return;

            if (flipPortal)
                FlipPortal();

            if (active)
                sourcePortal.SetLinkedPortal(portal_1);
            else
                sourcePortal.SetLinkedPortal(portal_0);
        }
    }
}