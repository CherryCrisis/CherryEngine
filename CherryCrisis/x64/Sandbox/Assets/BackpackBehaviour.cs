using CCEngine;

namespace CCScripting
{
	public class BackpackBehaviour : Behaviour
	{
		public Transform transform;

		public BackpackBehaviour(System.IntPtr cPtr, bool cMemoryOwn)
			: base(cPtr, cMemoryOwn)
		{
		}

		public void Start()
		{

			transform = GetComponent<Transform>();
			Debug.GetInstance().Log(ELogType.INFO, pos);
			transform.position = new Vector3(0f, 0f, -5f);
		}

		void SetRotation()
        {      
			if (!isMoving) 
				return; 
			 
				transform.eulerAngles = new Vector3(transform.eulerAngles.x, transform.eulerAngles.y, CherryEngine.Sin(time));
			
				//transform.eulerAngles = new Vector3(transform.eulerAngles.x, CherryEngine.Sin(time), transform.eulerAngles.z);
		}

		public Vector3 pos = new Vector3(5f, 3f, 7f);

		public float deltaTime = 0.01f;
		float time = 0f;
		int i = 0;
		int BMARIN = 0;    
		bool marinee = false;
		bool marine = true;
		bool isMoving = true;

		public void Update()
		{
			i++;
			time += Time.GetInstance().GetDeltaTime();

			SetRotation();

			float timeScaledSpeed = Time.GetInstance().GetDeltaTime() * 2f;

			transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z + InputManager.GetInstance().GetAxis("Horizontal") * timeScaledSpeed);
		}
	}
}
