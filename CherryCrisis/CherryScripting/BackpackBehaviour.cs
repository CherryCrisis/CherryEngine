using CCEngine;

namespace CCScripting
{
	public class BackpackBehaviour : Behaviour
	{
		public Transform transform;

		public BackpackBehaviour(System.IntPtr cPtr, bool cMemoryOwn)
			: base(cPtr, cMemoryOwn)
		{
			transform = GetComponent<Transform>();
		}

		public void Start()
		{
			Debug.GetInstance().Log(pos);
			InputManager.GetInstance().SetContext("user Context");
			transform.position = new Vector3(0f, 0f, -5f);
		}

		void SetRotation()
        {
			//transform.eulerAngles = new Vector3(transform.eulerAngles.x, CherryEngine.Sin(time), transform.eulerAngles.z);
			transform.eulerAngles = new Vector3(transform.eulerAngles.x, transform.eulerAngles.y, CherryEngine.Sin(time));

		}

		public Vector3 pos = new Vector3(5f, 3f, 7f);

		public string name = "Je suis un nom";
		public int num = 5;

		public float deltaTime = 0.01f;
		float time = 0f;
		public void Update()
		{
			time += 0.05f;

			SetRotation();


			float timeScaledSpeed = deltaTime * 2f;

			if (InputManager.GetInstance().GetKeyDown("Test"))
            {
				Debug.GetInstance().Log(transform.position, transform.eulerAngles, transform.scale);

			}

			if (InputManager.GetInstance().GetKey("Test"))
				transform.position = new Vector3(transform.position.x, transform.position.y + timeScaledSpeed, transform.position.z);

			if (InputManager.GetInstance().GetKey("Test2"))
				transform.position = new Vector3(transform.position.x, transform.position.y - timeScaledSpeed, transform.position.z);

			transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z + InputManager.GetInstance().GetAxis("Horizfeefeontal") * timeScaledSpeed);
		}
	}
}