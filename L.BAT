This directory contains example programs showing how to use the JNDI
API. They can be used to access any JNDI service provider you have
configured for your environment.

For expediency, these examples use the Java system properties for JNDI
environment properties.  This usage is not meant to be prescriptive
and is but one of the many ways to set up environment properties.

First, use JDK1.1.2 to compile all four .java files to generate
corresponding .class files.

To run these examples, make sure that your class path includes the
JNDI classes, the classes for your service providers, and the classes
of these examples.

When running the programs, you need to specify as system properties
java.naming.factory.initial and any environment properties expected by
your service provider. (See examples below).

You can then run these examples as follows (substitute the names of
the maps and entries appropriate for your environment).  $P denotes
the system property settings.

# java $P Attrs <name>
# java $P List [<name>]
# java $P List -b [<name>]
# java $P Search <name> [<attrid> <attrval>]*
# java $P Lookup <name>


--------------------------------------------------------
Examples accessing LDAP using the JNDI/LDAP provider:

LP="-Djava.naming.factory.initial=com.sun.jndi.ldap.LdapCtxFactory \
        -Djava.naming.provider.url=ldap://servername/o=widget,c=us"

Here are some examples of accessing LDAP using these examples:

To see a list of the top level LDAP namespace:
# java $LP List

----
To browse one of the lower levels:
# java $LP List ou=people

----
To get an object's attributes:
# java $LP Attrs "cn=John Smith,ou=people"

----
To search for an object with uid=jsmith:
# java $LP Search ou=people uid jsmith

---------------------------------------------------------
Examples access NIS using the JNDI/NIS provider:

NP="-Djava.naming.factory.initial=com.sun.jndi.nis.NISCtxFactory \
        -Djava.naming.provider.url=nis://servername/eng.widget.com"

To see a list of the initial context:
# java $NP List 

----
To see a list of the NIS maps:
# java $NP List system

----
You can use to Attrs program to discover the various attribute
identifiers assigned to the data in various maps. For example,

# java $NP Attrs system/passwd/jsmith

shows all the attributes associated with the entry jsmith in the
passwd map.

----
To search for an object with uid=jsmith:
# java $NP Search system/passwd uid jsmith

----
To provide consistency between information stored in NIS and information
stored in LDAP, the identifiers of the attributes used to represent NIS
maps are taken from internet draft "draft-ietf-asid-nis-schema-02".

-----------------------------------------------------------
Examples using the JNDI URL:

JNDI URLs may used for naming objects in a federated namespace rooted
at DNS.

JNDI URLs have the following syntax:
        jndi://dnsname/composite_name
This means that composite_name is to be resolved in the context named
by DNS record(s) associated with "dnsname".

If "dnsname" is omitted, as in this example:
        jndi:///composite_name
then composite_name is resolved in the initial context.  In this case the
three slashes may optionally be omitted, and the URL written as:
        jndi:composite_name

For an application to use the DNS federation aspect of JNDI URLs, it
must identify its DNS server by setting the java.naming.dns.url property:

JP="-Djava.naming.dns.url=dns://dnsserver/dnsdomain"

Assume that the records associated with "dns1" in the local DNS
domain indicate an LDAP server with root distinguished name
"o=widget,c=us".  Then:

# java $JP Attrs "jndi://dns1/cn=John Smith,ou=people"

retrieves the attributes associated with the name
cn=John Smith,ou=people,o=widget,c=us

----
The format currently being used for storing location information in DNS
is based upon the internet draft "draft-ietf-svrloc-advertising-02".
Following this model, the above example relies on a TXT record
being stored in the DNS node "dns1":

dns1 IN TXT service:j-ldap://servername/o=widget,c=us

----
Assume that the DNS node associated with "dns2" in your domain indicates
an NIS server for the domain widget.com.  Then:

# java $JP List jndi://dns2/system/passwd

will list the password entries in the widget.com NIS domain.

The TXT record stored in DNS at "dns2" for this example is:

dns2 IN TXT service:j-nis://servername/widget.com

-------------------------------------------------------------
Using policies to hide naming service dependencies:

With DNS configured as described above, the following searches will
find the attributes associated with user "jsmith" in a NIS domain and
in an LDAP directory, respectively:

# java $JP Search jndi://dns2/system/passwd uid jsmith
# java $JP Search jndi://dns1/ou=people uid jsmith

Although the use of federation through DNS hides some of the details
of which underlying service is being accessed, the different styles of
syntax used by the services gives some of it away.

To allow applications to be written in a more service-neutral style,
a context implementation may support the notion of a policy mapping.
Policies are used to translate a few service-neutral names into
names that may be used directly by the underlying services.

For example, the NIS provider hardwires the name "user" as an alias
for "system/passwd", so the following two searches are equivalent:

# java $JP Search jndi://dns2/system/passwd uid jsmith
# java $JP Search jndi://dns2/user uid jsmith

Rather than hardwiring policies, the LDAP provider accepts policies in
the form of an environment property "java.naming.ldap.policies".  Its value
is a string containing each policy component and its mapping onto LDAP.
The first character of the string is the delimiter used throughout the string.
For example, the name "user" may be mapped onto the LDAP name "ou=people":

POL=-Djava.naming.ldap.policies=:user:ou=people:

Then the following two searches are equivalent:

# java $JP $POL Search jndi://dns1/ou=people uid jsmith
# java $JP $POL Search jndi://dns1/user uid jsmith

Now, a user or application can search for user "jsmith" first in a NIS
domain, and then in an LDAP directory, in a generic manner:

# java $JP $POL Search jndi://dns2/user uid jsmith
# java $JP $POL Search jndi://dns1/user uid jsmith

-------------------------------------------------------------
