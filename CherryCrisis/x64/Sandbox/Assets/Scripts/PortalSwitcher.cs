using CCEngine;
using System;

namespace CCScripting
{
    public class PortalSwitcher : Behaviour
    {
        public PortalSwitcher(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

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
            emitter.Play();

            if (flipPortal)
                FlipPortal();

            if (active)
                sourcePortal.SetLinkedPortal(portal_1);
            else
                sourcePortal.SetLinkedPortal(portal_0);
        }
    }
}