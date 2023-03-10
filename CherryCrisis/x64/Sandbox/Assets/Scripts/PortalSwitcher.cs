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
        bool rescalePortals = false; 
        // if active then link portal_1
        bool active = false;

        private Transform transformSourcePortal;

        void Start()
        {
            emitter = AddBehaviour<AudioEmitter>();
            emitter.AddSound("Assets/Sounds/portal_switcher_interact.wav");
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
            active = !active;
            if (emitter != null)
                emitter.Play();

            if (portal_0 == null || portal_1 == null || sourcePortal == null)
                return;

            if (flipPortal)
                FlipPortal();

            //PortalComponent activePortal = active ? portal_1 : portal_0;

            if (active)
            {
                sourcePortal.SetLinkedPortal(portal_1);
                if (rescalePortals)
                    transformSourcePortal.scale = portal_1.GetBehaviour<Transform>().scale;
            }
            else              
            {
                sourcePortal.SetLinkedPortal(portal_0);
                if (rescalePortals)
                    transformSourcePortal.scale = portal_0.GetBehaviour<Transform>().scale;
            }
        }
    } 
}