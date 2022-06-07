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

        bool flipPortal = false;

        // if active then link portal_1
        bool active = false;

        private Transform transformSourcePortal;

        void Start()
        {
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

            if (flipPortal)
                FlipPortal();

            if (active)
                sourcePortal.SetLinkedPortal(portal_1);
            else
                sourcePortal.SetLinkedPortal(portal_0);
        }
    }
}