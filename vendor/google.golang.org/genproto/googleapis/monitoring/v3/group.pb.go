// Code generated by protoc-gen-go.
// source: google/monitoring/v3/group.proto
// DO NOT EDIT!

package monitoring

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// The description of a dynamic collection of monitored resources. Each group
// has a filter that is matched against monitored resources and their associated
// metadata. If a group's filter matches an available monitored resource, then
// that resource is a member of that group.  Groups can contain any number of
// monitored resources, and each monitored resource can be a member of any
// number of groups.
//
// Groups can be nested in parent-child hierarchies. The `parentName` field
// identifies an optional parent for each group.  If a group has a parent, then
// the only monitored resources available to be matched by the group's filter
// are the resources contained in the parent group.  In other words, a group
// contains the monitored resources that match its filter and the filters of all
// the group's ancestors.  A group without a parent can contain any monitored
// resource.
//
// For example, consider an infrastructure running a set of instances with two
// user-defined tags: `"environment"` and `"role"`. A parent group has a filter,
// `environment="production"`.  A child of that parent group has a filter,
// `role="transcoder"`.  The parent group contains all instances in the
// production environment, regardless of their roles.  The child group contains
// instances that have the transcoder role *and* are in the production
// environment.
//
// The monitored resources contained in a group can change at any moment,
// depending on what resources exist and what filters are associated with the
// group and its ancestors.
type Group struct {
	// Output only. The name of this group. The format is
	// `"projects/{project_id_or_number}/groups/{group_id}"`.
	// When creating a group, this field is ignored and a new name is created
	// consisting of the project specified in the call to `CreateGroup`
	// and a unique `{group_id}` that is generated automatically.
	Name string `protobuf:"bytes,1,opt,name=name" json:"name,omitempty"`
	// A user-assigned name for this group, used only for display purposes.
	DisplayName string `protobuf:"bytes,2,opt,name=display_name,json=displayName" json:"display_name,omitempty"`
	// The name of the group's parent, if it has one.
	// The format is `"projects/{project_id_or_number}/groups/{group_id}"`.
	// For groups with no parent, `parentName` is the empty string, `""`.
	ParentName string `protobuf:"bytes,3,opt,name=parent_name,json=parentName" json:"parent_name,omitempty"`
	// The filter used to determine which monitored resources belong to this group.
	Filter string `protobuf:"bytes,5,opt,name=filter" json:"filter,omitempty"`
	// If true, the members of this group are considered to be a cluster.
	// The system can perform additional analysis on groups that are clusters.
	IsCluster bool `protobuf:"varint,6,opt,name=is_cluster,json=isCluster" json:"is_cluster,omitempty"`
}

func (m *Group) Reset()                    { *m = Group{} }
func (m *Group) String() string            { return proto.CompactTextString(m) }
func (*Group) ProtoMessage()               {}
func (*Group) Descriptor() ([]byte, []int) { return fileDescriptor1, []int{0} }

func (m *Group) GetName() string {
	if m != nil {
		return m.Name
	}
	return ""
}

func (m *Group) GetDisplayName() string {
	if m != nil {
		return m.DisplayName
	}
	return ""
}

func (m *Group) GetParentName() string {
	if m != nil {
		return m.ParentName
	}
	return ""
}

func (m *Group) GetFilter() string {
	if m != nil {
		return m.Filter
	}
	return ""
}

func (m *Group) GetIsCluster() bool {
	if m != nil {
		return m.IsCluster
	}
	return false
}

func init() {
	proto.RegisterType((*Group)(nil), "google.monitoring.v3.Group")
}

func init() { proto.RegisterFile("google/monitoring/v3/group.proto", fileDescriptor1) }

var fileDescriptor1 = []byte{
	// 248 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x6c, 0x90, 0xc1, 0x4a, 0x03, 0x31,
	0x10, 0x86, 0x49, 0xb5, 0x8b, 0x9d, 0x7a, 0x0a, 0x22, 0x8b, 0x20, 0xae, 0x9e, 0x7a, 0x4a, 0x0e,
	0x7b, 0x14, 0x3c, 0xb4, 0x87, 0x9e, 0x94, 0xd2, 0x83, 0x07, 0x2f, 0x25, 0xb6, 0x31, 0x04, 0xb2,
	0x99, 0x90, 0xec, 0x2e, 0xf8, 0x00, 0xbe, 0x82, 0x0f, 0xe1, 0x53, 0xca, 0x4e, 0x16, 0x16, 0xc1,
	0x5b, 0xe6, 0xff, 0x3e, 0x26, 0x33, 0x03, 0x95, 0x41, 0x34, 0x4e, 0xcb, 0x06, 0xbd, 0x6d, 0x31,
	0x5a, 0x6f, 0x64, 0x5f, 0x4b, 0x13, 0xb1, 0x0b, 0x22, 0x44, 0x6c, 0x91, 0x5f, 0x65, 0x43, 0x4c,
	0x86, 0xe8, 0xeb, 0x87, 0x6f, 0x06, 0xf3, 0xed, 0x60, 0x71, 0x0e, 0xe7, 0x5e, 0x35, 0xba, 0x64,
	0x15, 0x5b, 0x2d, 0xf6, 0xf4, 0xe6, 0xf7, 0x70, 0x79, 0xb2, 0x29, 0x38, 0xf5, 0x79, 0x20, 0x36,
	0x23, 0xb6, 0x1c, 0xb3, 0x97, 0x41, 0xb9, 0x83, 0x65, 0x50, 0x51, 0xfb, 0x36, 0x1b, 0x67, 0x64,
	0x40, 0x8e, 0x48, 0xb8, 0x86, 0xe2, 0xc3, 0xba, 0x56, 0xc7, 0x72, 0x4e, 0x6c, 0xac, 0xf8, 0x2d,
	0x80, 0x4d, 0x87, 0xa3, 0xeb, 0xd2, 0xc0, 0x8a, 0x8a, 0xad, 0x2e, 0xf6, 0x0b, 0x9b, 0x36, 0x39,
	0x58, 0x7f, 0x31, 0x28, 0x8f, 0xd8, 0x88, 0xff, 0xa6, 0x5e, 0x03, 0x8d, 0xbc, 0x1b, 0xf6, 0xda,
	0xb1, 0xb7, 0xa7, 0xd1, 0x31, 0xe8, 0x94, 0x37, 0x02, 0xa3, 0x91, 0x46, 0x7b, 0xda, 0x5a, 0x66,
	0xa4, 0x82, 0x4d, 0x7f, 0x4f, 0xf3, 0x38, 0x55, 0x3f, 0xb3, 0x9b, 0x6d, 0x6e, 0xb0, 0x71, 0xd8,
	0x9d, 0xc4, 0xf3, 0xf4, 0xd5, 0x6b, 0xfd, 0x5e, 0x50, 0x9f, 0xfa, 0x37, 0x00, 0x00, 0xff, 0xff,
	0x0c, 0x22, 0x2a, 0x57, 0x61, 0x01, 0x00, 0x00,
}