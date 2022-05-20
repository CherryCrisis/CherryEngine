using CCEngine;

namespace CCScripting
{
    public class Script : Behaviour 
    {
        public Script(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        public void OnClick() 
        {
            SceneManager.ChangeScene("Empty2.ccscene");
        }
    }
}