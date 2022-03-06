using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CCEngine;

namespace CherryScripting
{ 
	public class MyComponent : Behaviour
	{
		public MyComponent(Entity owner) 
			: base(owner)
		{
			Console.WriteLine(owner);
		}
			
		int i = 5;

		public void Update()
		{
			i++;
			
			//Console.WriteLine(host);
		}
	}
}
