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

        bool hasSwitched = false;

        private Transform transformSourcePortal;
        void Start()
        {
            emitter = AddBehaviour<AudioEmitter>();
            emitter.AddSound("Assets/Sounds/receiver_switch.wav");
            emitter.SetSpatialized(false);
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
            if (hasSwitched)
                return;

            active = !active;
            hasSwitched = true;
            if (emitter != null)
                emitter.Play();

            if (portal_0 == null || portal_1 == null || sourcePortal == null)
                return;

            if (flipPortal)
                FlipPortal();

            PortalComponent activePortal = active ? portal_1 : portal_0;
            sourcePortal.SetLinkedPortal(activePortal);
        }
    }
}