using CCEngine;

namespace CCScripting
{
    public class PortalSwitcher : Behaviour
    {
        public PortalSwitcher(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

        PortalComponent sourcePortal;
        PortalComponent portal_0;
        PortalComponent portal_1;

        // if active then link portal_1
        bool active = false; 

        public void Switch() 
        {
            active = !active;

            if (active)
                sourcePortal.SetLinkedPortal(portal_1);
            else
                sourcePortal.SetLinkedPortal(portal_0);
        }
    }
}