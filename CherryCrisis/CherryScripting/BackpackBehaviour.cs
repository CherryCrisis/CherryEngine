using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using CCEngine;

namespace CCScripting
{
	class BackpackBehaviour : Behaviour
	{
		public BackpackBehaviour(System.IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn) { }

		float i = 0.5f;

		Transform transform;

		public void Start()
		{
			transform = GetComponent<Transform>();

			transform.position = new Vector3(transform.position.x, transform.position.y, -50f);
		}

		public void Update()
		{
			i += 0.1f;
			transform.eulerAngles = new Vector3(transform.eulerAngles.x, i, transform.eulerAngles.z);

		}
	}
}