using CCEngine;

namespace CCScripting
{
    public class CloseButton : Behaviour 
    {
        public CloseButton(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        public void OnClick()  
        {
            Engine.Quit();
            Debug.Log(ELogType.INFO, "Killing the game");
        }
    }
}