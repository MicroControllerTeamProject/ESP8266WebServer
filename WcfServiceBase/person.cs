using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Web;

namespace WcfServiceBase
{
    [DataContract]
    public class Person
    {
        [DataMember]
        public string name { get; set; }
        [DataMember]
        public string username { get; set; }
        [DataMember]
        public string email { get; set; }
    }
}