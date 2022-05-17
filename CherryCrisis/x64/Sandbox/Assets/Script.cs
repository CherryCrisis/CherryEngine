using CCEngine;

namespace CCScripting
{
    public class Script : Behaviour 
    {
        public Script(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        public void OnClick() 
        {
            Debug.GetInstance().Log(ELogType.INFO, "Button Reacted from csharp !"); 
        }
    }
}