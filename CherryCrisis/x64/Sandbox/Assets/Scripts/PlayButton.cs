using CCEngine;

namespace CCScripting
{
    public class PlayButton : Behaviour 
    {
        public PlayButton(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        public void OnClick() 
        {
            SceneManager.ChangeScene("Scene/Level_01"); 
        }
    }
}