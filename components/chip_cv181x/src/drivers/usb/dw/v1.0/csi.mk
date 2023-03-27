 #
 #   http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

INCDIR += -I$(CHIPDIR)/include
INCDIR += -I$(DRIVERDIR)/include
USBDIR = $(DRIVERDIR)/usb

ifeq ($(CONFIG_CHIP_TX216)$(CONFIG_CHIP_CETUS), y)
INCDIR += -I$(LIBSDIR)/device
INCDIR += -I$(LIBSDIR)/host

INCDIR += -I$(LIBSDIR)/usb_lib/include/XBASE
INCDIR += -I$(LIBSDIR)/usb_lib/include/XUSBD
INCDIR += -I$(LIBSDIR)/usb_lib/include/XUSBD/Core
INCDIR += -I$(LIBSDIR)/usb_lib/include/XUSBD/Function

INCDIR += -I$(LIBSDIR)/usb_lib/include/XUSBH/MassStor/ \
          -I$(LIBSDIR)/usb_lib/include/XUSBH/ \
          -I$(LIBSDIR)/usb_lib/include/XUSBH/Hub \
          -I$(LIBSDIR)/usb_lib/include/XUSBH/HCD \
          -I$(LIBSDIR)/usb_lib/include/XUSBH/USBD \
          -I$(LIBSDIR)/usb_lib/include/XFS \


DRIVER_CSRC += $(LIBSDIR)/host/usyndwc.c
DRIVER_CSRC += $(LIBSDIR)/device/dwc2_usbd.c
DRIVER_CSRC += $(LIBSDIR)/usb_lib/config/boscsky.c
DRIVER_CSRC += $(LIBSDIR)/usb_lib/config/udheap.c
DRIVER_CSRC += $(LIBSDIR)/usb_lib/config/bcc.c
endif
