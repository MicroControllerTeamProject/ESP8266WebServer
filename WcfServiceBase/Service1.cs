using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace WcfServiceBase
{
    public class Service1 : IService1
    {
        public Person GetData()
        {
            return new Person() { name = "luigi", username = "hp9000", email = "tin@it" };
        }

        public   void GetDataUsingDataContract(Person a)
        {
            
        }
    }
}