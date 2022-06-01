using CCEngine;

namespace CCScripting
{
    public class PortalModifier : Behaviour
    {
        public PortalModifier(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) { }


        PortalModifier otherPortal;

        //called at the start of the game
        public void Awake()
        {
            //model = GetBehaviour<ModelRenderer>();
        }
        public void Start()
        {
            GetBehaviour<PortalComponent>().SetLinkedPortal(otherPortal);
            //model.m_mesh = GetBehaviour<Transform>().GetParent().host.GetBehaviour<ModelRenderer>().m_mesh;
        }


        //called each tick 
        public void Update()
        {

        }
    }
}