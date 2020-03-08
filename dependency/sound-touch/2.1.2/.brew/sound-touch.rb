class SoundTouch < Formula
  desc "Audio processing library"
  homepage "https://www.surina.net/soundtouch/"
  url "https://gitlab.com/soundtouch/soundtouch/-/archive/2.1.2/soundtouch-2.1.2.tar.gz"
  sha256 "2826049e2f34efbc4c8a47d00c93649822b0c14e1f29f5569835704814590732"

  depends_on "autoconf" => :build
  depends_on "automake" => :build
  depends_on "libtool" => :build

  def install
    system "/bin/sh", "bootstrap"
    args = %W[
      --disable-dependency-tracking
      --disable-silent-rules
      --prefix=#{prefix}
    ]

    system "./configure", *args
    system "make", "install"
  end

  test do
    assert_match /SoundStretch v#{version} -/, shell_output("#{bin}/soundstretch 2>&1", 255)
  end
end
