using CCEngine;

namespace CCScripting
{
    public class MeshModifier : Behaviour
    {
        public MeshModifier(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        ModelRenderer model;

        //called at the start of the game
        public void Awake()
        {
            model = GetBehaviour<ModelRenderer>();

            //model = GetBehaviour<ModelRenderer>();
        }
        public void Start()
		{ 
            //model.m_mesh = GetBehaviour<Transform>().GetParent().host.GetBehaviour<ModelRenderer>().m_mesh;
        }


        //called each tick 
        public void Update()
        {
            
        }
    }
}